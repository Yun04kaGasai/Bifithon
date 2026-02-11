#include "BIFtkinter.h"

#include <windows.h>
#include <unordered_map>
#include <vector>

namespace bif {
namespace tkinter {

static std::unordered_map<HWND, int> g_last_click;

static LRESULT CALLBACK BIFWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_COMMAND) {
        if (HIWORD(wparam) == BN_CLICKED) {
            int control_id = LOWORD(wparam);
            g_last_click[hwnd] = control_id;
            return 0;
        }
    }

    if (message == WM_DESTROY) {
        g_last_click.erase(hwnd);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, message, wparam, lparam);
}

BIFWindow::BIFWindow(const std::string& title, int width, int height)
    : title_(title), width_(width), height_(height), hwnd_(nullptr), next_id_(100) {
    static bool class_registered = false;
    if (!class_registered) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = BIFWindowProc;
        wc.hInstance = GetModuleHandleA(nullptr);
        wc.lpszClassName = "BIFtkinterWindow";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        RegisterClassA(&wc);
        class_registered = true;
    }

    HWND hwnd = CreateWindowExA(
        0,
        "BIFtkinterWindow",
        title_.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width_,
        height_,
        nullptr,
        nullptr,
        GetModuleHandleA(nullptr),
        nullptr);

    hwnd_ = hwnd;
}

void BIFWindow::show() {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

int BIFWindow::wait_for_click() {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return -1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        auto it = g_last_click.find(hwnd);
        if (it != g_last_click.end()) {
            int clicked_id = it->second;
            g_last_click.erase(it);
            return clicked_id;
        }
    }

    return -1;
}

void BIFWindow::set_title(const std::string& title) {
    title_ = title;
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (hwnd) {
        SetWindowTextA(hwnd, title_.c_str());
    }
}

void BIFWindow::set_size(int width, int height) {
    width_ = width;
    height_ = height;
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (hwnd) {
        SetWindowPos(hwnd, nullptr, 0, 0, width_, height_, SWP_NOMOVE | SWP_NOZORDER);
    }
}

int BIFWindow::add_label(const std::string& text, int x, int y, int width, int height) {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return -1;
    }

    int id = next_id_++;
    CreateWindowExA(
        0,
        "STATIC",
        text.c_str(),
        WS_CHILD | WS_VISIBLE,
        x,
        y,
        width,
        height,
        hwnd,
        reinterpret_cast<HMENU>(static_cast<intptr_t>(id)),
        GetModuleHandleA(nullptr),
        nullptr);
    return id;
}

int BIFWindow::add_button(const std::string& text, int x, int y, int width, int height) {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return -1;
    }

    int id = next_id_++;
    CreateWindowExA(
        0,
        "BUTTON",
        text.c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x,
        y,
        width,
        height,
        hwnd,
        reinterpret_cast<HMENU>(static_cast<intptr_t>(id)),
        GetModuleHandleA(nullptr),
        nullptr);
    return id;
}

int BIFWindow::add_input(int x, int y, int width, int height) {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return -1;
    }

    int id = next_id_++;
    CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        x,
        y,
        width,
        height,
        hwnd,
        reinterpret_cast<HMENU>(static_cast<intptr_t>(id)),
        GetModuleHandleA(nullptr),
        nullptr);
    return id;
}

std::string BIFWindow::get_input_text(int control_id) const {
    HWND hwnd = static_cast<HWND>(hwnd_);
    if (!hwnd) {
        return "";
    }

    HWND control = GetDlgItem(hwnd, control_id);
    if (!control) {
        return "";
    }

    int length = GetWindowTextLengthA(control);
    if (length <= 0) {
        return "";
    }

    std::vector<char> buffer(static_cast<size_t>(length) + 1, '\0');
    GetWindowTextA(control, buffer.data(), length + 1);
    return std::string(buffer.data());
}

} // namespace tkinter
} // namespace bif
