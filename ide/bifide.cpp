#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/splitter.h>
#include <wx/stdpaths.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static wxString QuoteArg(const wxString& value) {
    if (value.Find(' ') == wxNOT_FOUND && value.Find('\t') == wxNOT_FOUND) {
        return value;
    }
    wxString escaped = value;
    escaped.Replace("\"", "\\\"");
    return "\"" + escaped + "\"";
}

static fs::path FindRepoRoot() {
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    fs::path current = fs::absolute(exePath.ToStdString()).parent_path();

    while (!current.empty()) {
        if (fs::exists(current / "tools")) {
            return current;
        }
        fs::path parent = current.parent_path();
        if (parent == current) {
            break;
        }
        current = parent;
    }

    return fs::current_path();
}

class InputDialog final : public wxDialog {
public:
    explicit InputDialog(wxWindow* parent)
        : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Ввод"), wxDefaultPosition, wxSize(500, 300)) {
        wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* label = new wxStaticText(
            this,
            wxID_ANY,
            wxString::FromUTF8("Введите данные для input() (каждая строка — отдельный ввод):"));
        rootSizer->Add(label, 0, wxALL, 8);

        inputCtrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        rootSizer->Add(inputCtrl_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

        wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
        buttonSizer->AddButton(new wxButton(this, wxID_OK, wxString::FromUTF8("OK")));
        buttonSizer->AddButton(new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Отмена")));
        buttonSizer->Realize();
        rootSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 8);

        SetSizerAndFit(rootSizer);
    }

    wxString GetInput() const {
        return inputCtrl_->GetValue();
    }

private:
    wxTextCtrl* inputCtrl_ = nullptr;
};

class BifideFrame final : public wxFrame {
public:
    BifideFrame()
        : wxFrame(nullptr, wxID_ANY, "BiIDE", wxDefaultPosition, wxSize(900, 600)),
          repoRoot_(FindRepoRoot()) {
        CreateStatusBar();
        SetStatusText(wxString::FromUTF8("Готово"));

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* openButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8("Открыть"));
        wxButton* runButton = new wxButton(panel, wxID_ANY, wxString::FromUTF8("Запуск"));
        toolbarSizer->Add(openButton, 0, wxALL, 4);
        toolbarSizer->Add(runButton, 0, wxALL, 4);
        rootSizer->Add(toolbarSizer, 0, wxLEFT | wxTOP, 4);

        wxSplitterWindow* splitter = new wxSplitterWindow(panel, wxID_ANY);
        editor_ = new wxTextCtrl(splitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
        output_ = new wxTextCtrl(splitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
        splitter->SplitHorizontally(editor_, output_, -200);
        rootSizer->Add(splitter, 1, wxEXPAND | wxALL, 4);

        panel->SetSizer(rootSizer);

        Bind(wxEVT_MENU, &BifideFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_MENU, &BifideFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &BifideFrame::OnSaveAs, this, wxID_SAVEAS);
        Bind(wxEVT_MENU, &BifideFrame::OnRun, this, wxID_EXECUTE);
        Bind(wxEVT_MENU, &BifideFrame::OnExit, this, wxID_EXIT);

        openButton->Bind(wxEVT_BUTTON, &BifideFrame::OnOpen, this);
        runButton->Bind(wxEVT_BUTTON, &BifideFrame::OnRun, this);

        editor_->Bind(wxEVT_CHAR_HOOK, &BifideFrame::OnEditorChar, this);

        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(wxID_OPEN, wxString::FromUTF8("Открыть"));
        fileMenu->Append(wxID_SAVE, wxString::FromUTF8("Сохранить"));
        fileMenu->Append(wxID_SAVEAS, wxString::FromUTF8("Сохранить как"));
        fileMenu->AppendSeparator();
        fileMenu->Append(1001, wxString::FromUTF8("Пример: Деление"));
        fileMenu->Append(1002, wxString::FromUTF8("Пример: If/Else"));
        fileMenu->Append(1003, wxString::FromUTF8("Пример: While"));
        fileMenu->Append(1004, wxString::FromUTF8("Пример: Ввод"));
        fileMenu->Append(1005, wxString::FromUTF8("Пример: Tkinter"));
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXECUTE, wxString::FromUTF8("Запуск"));
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXIT, wxString::FromUTF8("Выход"));

        wxMenuBar* menuBar = new wxMenuBar();
        menuBar->Append(fileMenu, wxString::FromUTF8("Файл"));
        SetMenuBar(menuBar);

        Bind(wxEVT_MENU, &BifideFrame::OnExample, this, 1001, 1005);
    }

private:
    void OnOpen(wxCommandEvent&) {
        wxFileDialog dialog(
            this,
            wxString::FromUTF8("Открыть файл Bifithon"),
            "",
            "",
            wxString::FromUTF8("Bifithon files (*.bif)|*.bif|All files (*.*)|*.*"),
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dialog.ShowModal() != wxID_OK) {
            return;
        }
        LoadFile(dialog.GetPath());
    }

    void OnSave(wxCommandEvent&) {
        if (currentPath_.empty()) {
            OnSaveAs(wxCommandEvent());
            return;
        }
        SaveFile(currentPath_);
    }

    void OnSaveAs(wxCommandEvent&) {
        wxFileDialog dialog(
            this,
            wxString::FromUTF8("Сохранить файл Bifithon"),
            "",
            "",
            wxString::FromUTF8("Bifithon files (*.bif)|*.bif|All files (*.*)|*.*"),
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dialog.ShowModal() != wxID_OK) {
            return;
        }
        SaveFile(dialog.GetPath());
    }

    void OnRun(wxCommandEvent&) {
        if (!EnsureSaved()) {
            return;
        }

        wxString source = editor_->GetValue();
        wxString stdinData;
        if (source.Find("input(") != wxNOT_FOUND) {
            InputDialog dialog(this);
            if (dialog.ShowModal() != wxID_OK) {
                SetStatusText(wxString::FromUTF8("Запуск отменен"));
                return;
            }
            stdinData = dialog.GetInput();
        }

        wxString compiler = FindCompiler();
        if (compiler.empty()) {
            wxMessageBox(
                wxString::FromUTF8("Компилятор не найден: tools/bifc или tools/bifc.exe"),
                "Bifithon",
                wxOK | wxICON_ERROR);
            SetStatusText(wxString::FromUTF8("Компилятор не найден"));
            return;
        }

        wxString command = BuildCommand(compiler, stdinData);
        SetStatusText(wxString::FromUTF8("Выполнение..."));

        wxArrayString out;
        wxArrayString err;
        long code = wxExecute(command, out, err, wxEXEC_SYNC);

        wxString combined;
        for (const auto& line : out) {
            combined += line + "\n";
        }
        for (const auto& line : err) {
            combined += line + "\n";
        }
        output_->SetValue(combined);

        if (code == 0) {
            SetStatusText(wxString::FromUTF8("Готово"));
        } else {
            SetStatusText(wxString::FromUTF8("Ошибка"));
        }
    }

    void OnExit(wxCommandEvent&) {
        Close(true);
    }

    void OnExample(wxCommandEvent& event) {
        wxString fileName;
        switch (event.GetId()) {
            case 1001:
                fileName = "division.bif";
                break;
            case 1002:
                fileName = "if_else.bif";
                break;
            case 1003:
                fileName = "while_loop.bif";
                break;
            case 1004:
                fileName = "input_echo.bif";
                break;
            case 1005:
                fileName = "tkinter_demo.bif";
                break;
            default:
                return;
        }

        fs::path examplePath = repoRoot_ / "examples" / fileName.ToStdString();
        if (!fs::exists(examplePath)) {
            wxMessageBox(
                wxString::FromUTF8("Пример не найден: ") + fileName,
                "BiIDE",
                wxOK | wxICON_ERROR);
            return;
        }

        LoadFile(examplePath.string());
        SetStatusText(wxString::FromUTF8("Открыт пример"));
    }

    void OnEditorChar(wxKeyEvent& event) {
        int key = event.GetKeyCode();
        if (key == WXK_RETURN || key == WXK_NUMPAD_ENTER) {
            long pos = editor_->GetInsertionPoint();
            long lineNo = editor_->LineFromPosition(pos);
            wxString lineText = editor_->GetLineText(lineNo);

            int indent = 0;
            for (wxUniChar ch : lineText) {
                if (ch == ' ') {
                    indent += 1;
                } else if (ch == '\t') {
                    indent += 4;
                } else {
                    break;
                }
            }

            wxString trimmed = lineText;
            trimmed.Trim(true).Trim(false);
            int extra = trimmed.EndsWith(":") ? 4 : 0;
            editor_->WriteText("\n" + wxString(' ', indent + extra));
            return;
        }

        if (key == WXK_TAB) {
            editor_->WriteText("    ");
            return;
        }

        event.Skip();
    }

    void LoadFile(const wxString& path) {
        std::ifstream in(path.ToStdString(), std::ios::binary);
        std::ostringstream ss;
        ss << in.rdbuf();
        editor_->SetValue(ss.str());
        currentPath_ = path;
        SetTitle("BiIDE - " + wxFileName(path).GetFullName());
        SetStatusText(wxString::FromUTF8("Файл открыт"));
    }

    bool SaveFile(const wxString& path) {
        std::ofstream out(path.ToStdString(), std::ios::binary);
        std::string data = editor_->GetValue().ToStdString();
        out.write(data.data(), static_cast<std::streamsize>(data.size()));
        currentPath_ = path;
        SetTitle("BiIDE - " + wxFileName(path).GetFullName());
        SetStatusText(wxString::FromUTF8("Сохранено"));
        return true;
    }

    bool EnsureSaved() {
        if (!currentPath_.empty()) {
            return SaveFile(currentPath_);
        }
        wxCommandEvent dummy;
        OnSaveAs(dummy);
        return !currentPath_.empty();
    }

    wxString FindCompiler() const {
        fs::path exePath = repoRoot_ / "tools" / "bifc.exe";
        if (fs::exists(exePath)) {
            return exePath.wstring();
        }
        fs::path binPath = repoRoot_ / "tools" / "bifc";
        if (fs::exists(binPath)) {
            return binPath.wstring();
        }
        return "";
    }

    wxString BuildCommand(const wxString& compiler, const wxString& stdinData) {
        wxString base = QuoteArg(compiler) + " " + QuoteArg(currentPath_) + " --run";

        if (stdinData.empty()) {
            return base;
        }

        wxString tempPath = wxFileName::CreateTempFileName("bif_input");
        {
            std::ofstream out(tempPath.ToStdString(), std::ios::binary);
            std::string data = stdinData.ToStdString();
            out.write(data.data(), static_cast<std::streamsize>(data.size()));
        }

#ifdef _WIN32
        wxString redirect = base + " < " + QuoteArg(tempPath);
        return "cmd /c " + QuoteArg(redirect);
#else
        wxString redirect = base + " < " + QuoteArg(tempPath);
        return "/bin/sh -c " + QuoteArg(redirect);
#endif
    }

    wxTextCtrl* editor_ = nullptr;
    wxTextCtrl* output_ = nullptr;
    wxString currentPath_;
    fs::path repoRoot_;
};

class BifideApp final : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new BifideFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(BifideApp);
