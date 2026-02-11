#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

struct ParseError {
    std::string message;
};

std::string strip_comment(const std::string& line) {
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;
    std::string out;

    for (char ch : line) {
        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            continue;
        }

        if (ch == '"' || ch == '\'') {
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            continue;
        }

        if (ch == '#') {
            break;
        }

        out.push_back(ch);
    }

    return out;
}

std::string replace_keywords(const std::string& expr) {
    std::unordered_map<std::string, std::string> replacements = {
        {"and", "&&"},
        {"or", "||"},
        {"not", "!"},
        {"True", "true"},
        {"False", "false"},
        {"None", "nullptr"},
    };

    std::string out;
    std::string word;
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;

    auto flush_word = [&]() {
        if (!word.empty()) {
            auto it = replacements.find(word);
            out += (it != replacements.end()) ? it->second : word;
            word.clear();
        }
    };

    for (char ch : expr) {
        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            continue;
        }

        if (ch == '"' || ch == '\'') {
            flush_word();
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            continue;
        }

        if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '_') {
            word.push_back(ch);
        } else {
            flush_word();
            out.push_back(ch);
        }
    }

    flush_word();
    return out;
}

bool expr_has_division(const std::string& expr) {
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;

    for (char ch : expr) {
        if (in_string) {
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            continue;
        }

        if (ch == '"' || ch == '\'') {
            in_string = true;
            string_char = ch;
            continue;
        }

        if (ch == '/') {
            return true;
        }
    }

    return false;
}

std::string promote_int_literals_for_division(const std::string& expr) {
    std::string out;
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;
    size_t i = 0;

    while (i < expr.size()) {
        char ch = expr[i];

        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            ++i;
            continue;
        }

        if (ch == '"' || ch == '\'') {
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            ++i;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch)) &&
            (i == 0 || !(std::isalnum(static_cast<unsigned char>(expr[i - 1])) || expr[i - 1] == '_'))) {
            size_t start = i;
            while (i < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i]))) {
                ++i;
            }

            bool is_float = false;
            if (i < expr.size() && expr[i] == '.') {
                is_float = true;
                ++i;
                while (i < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i]))) {
                    ++i;
                }
            }

            if (i < expr.size() && (expr[i] == 'e' || expr[i] == 'E')) {
                is_float = true;
                ++i;
                if (i < expr.size() && (expr[i] == '+' || expr[i] == '-')) {
                    ++i;
                }
                while (i < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i]))) {
                    ++i;
                }
            }

            std::string token = expr.substr(start, i - start);
            if (!is_float) {
                token += ".0";
            }
            out += token;
            continue;
        }

        out.push_back(ch);
        ++i;
    }

    return out;
}

std::string replace_input_calls(const std::string& expr) {
    std::string out;
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;
    size_t i = 0;

    while (i < expr.size()) {
        char ch = expr[i];

        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            ++i;
            continue;
        }

        if (ch == '"' || ch == '\'') {
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            ++i;
            continue;
        }

        if (expr.compare(i, 6, "input(") == 0) {
            out += "bif_input(";
            i += 6;
            continue;
        }

        out.push_back(ch);
        ++i;
    }

    return out;
}

std::string replace_module_access(const std::string& expr, const std::vector<std::string>& modules) {
    std::string out;
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;
    size_t i = 0;

    while (i < expr.size()) {
        char ch = expr[i];

        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            ++i;
            continue;
        }

        if (ch == '"' || ch == '\'') {
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            ++i;
            continue;
        }

        bool matched = false;
        for (const auto& module_name : modules) {
            std::string token = module_name + ".";
            if (expr.compare(i, token.size(), token) == 0) {
                out += module_name + "::";
                i += token.size();
                matched = true;
                break;
            }
        }

        if (matched) {
            continue;
        }

        out.push_back(ch);
        ++i;
    }

    return out;
}

std::string replace_imported_names(const std::string& expr, const std::unordered_map<std::string, std::string>& imported) {
    std::string out;
    std::string word;
    bool in_string = false;
    char string_char = '\0';
    bool escaped = false;

    auto flush_word = [&]() {
        if (word.empty()) {
            return;
        }
        auto it = imported.find(word);
        if (it != imported.end()) {
            out += it->second + "::" + word;
        } else {
            out += word;
        }
        word.clear();
    };

    for (char ch : expr) {
        if (in_string) {
            out.push_back(ch);
            if (escaped) {
                escaped = false;
            } else if (ch == '\\') {
                escaped = true;
            } else if (ch == string_char) {
                in_string = false;
            }
            continue;
        }

        if (ch == '"' || ch == '\'') {
            flush_word();
            in_string = true;
            string_char = ch;
            out.push_back(ch);
            continue;
        }

        if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '_') {
            word.push_back(ch);
        } else {
            flush_word();
            out.push_back(ch);
        }
    }

    flush_word();
    return out;
}

std::string normalize_expression(
    const std::string& expr,
    const std::vector<std::string>& modules,
    const std::unordered_map<std::string, std::string>& imported_names) {
    std::string out = replace_keywords(expr);
    out = replace_input_calls(out);
    if (!modules.empty()) {
        out = replace_module_access(out, modules);
    }
    if (!imported_names.empty()) {
        out = replace_imported_names(out, imported_names);
    }
    if (expr_has_division(out)) {
        out = promote_int_literals_for_division(out);
    }
    return out;
}

bool is_valid_identifier(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    if (!(std::isalpha(static_cast<unsigned char>(name[0])) || name[0] == '_')) {
        return false;
    }
    for (char ch : name) {
        if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_')) {
            return false;
        }
    }
    return true;
}

std::pair<std::string, std::string> split_assignment(const std::string& line) {
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] != '=') {
            continue;
        }
        char prev = (i > 0) ? line[i - 1] : '\0';
        char next = (i + 1 < line.size()) ? line[i + 1] : '\0';
        if (prev == '=' || prev == '!' || prev == '<' || prev == '>' || next == '=') {
            continue;
        }
        return {line.substr(0, i), line.substr(i + 1)};
    }
    return {"", ""};
}

struct TranspileResult {
    std::vector<std::string> body;
    std::vector<std::string> imports;
};

TranspileResult transpile_bif(const std::vector<std::string>& lines) {
    std::vector<std::string> out;
    std::vector<int> indent_stack = {0};
    bool expect_indent = false;
    std::unordered_set<std::string> defined;
    std::vector<std::string> imports;
    std::unordered_map<std::string, std::string> imported_names;
    std::unordered_set<std::string> known_imports = {"BIFMath", "BIFitertools", "BIFtkinter"};

    for (size_t index = 0; index < lines.size(); ++index) {
        int lineno = static_cast<int>(index) + 1;
        std::string line = strip_comment(lines[index]);
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }
        if (line.find('\t') != std::string::npos) {
            throw ParseError{"Line " + std::to_string(lineno) + ": Tabs are not allowed. Use 4 spaces."};
        }
        auto first_non_space = line.find_first_not_of(' ');
        if (first_non_space == std::string::npos) {
            continue;
        }
        int indent = static_cast<int>(first_non_space);
        if (indent % 4 != 0) {
            throw ParseError{"Line " + std::to_string(lineno) + ": Indentation must be multiples of 4 spaces."};
        }
        if (indent > indent_stack.back()) {
            if (!expect_indent) {
                throw ParseError{"Line " + std::to_string(lineno) + ": Unexpected indentation."};
            }
            indent_stack.push_back(indent);
            expect_indent = false;
        }
        while (indent < indent_stack.back()) {
            out.push_back("}");
            indent_stack.pop_back();
        }
        if (expect_indent && indent == indent_stack.back()) {
            throw ParseError{"Line " + std::to_string(lineno) + ": Expected indented block."};
        }

        std::string stripped = line.substr(first_non_space);

        if (stripped.rfind("import ", 0) == 0) {
            std::string module_name = stripped.substr(7);
            module_name.erase(0, module_name.find_first_not_of(' '));
            if (known_imports.find(module_name) == known_imports.end()) {
                throw ParseError{"Line " + std::to_string(lineno) + ": Unknown module '" + module_name + "'."};
            }
            if (std::find(imports.begin(), imports.end(), module_name) == imports.end()) {
                imports.push_back(module_name);
            }
            continue;
        }

        if (stripped.rfind("from ", 0) == 0) {
            size_t pos = stripped.find(" import ");
            if (pos == std::string::npos) {
                throw ParseError{"Line " + std::to_string(lineno) + ": Invalid import syntax."};
            }
            std::string module_name = stripped.substr(5, pos - 5);
            module_name.erase(0, module_name.find_first_not_of(' '));
            module_name.erase(module_name.find_last_not_of(' ') + 1);
            if (known_imports.find(module_name) == known_imports.end()) {
                throw ParseError{"Line " + std::to_string(lineno) + ": Unknown module '" + module_name + "'."};
            }
            if (std::find(imports.begin(), imports.end(), module_name) == imports.end()) {
                imports.push_back(module_name);
            }
            std::string names_part = stripped.substr(pos + 8);
            std::stringstream ss(names_part);
            std::string name;
            bool has_name = false;
            while (std::getline(ss, name, ',')) {
                name.erase(0, name.find_first_not_of(' '));
                name.erase(name.find_last_not_of(' ') + 1);
                if (name.empty()) {
                    continue;
                }
                has_name = true;
                if (!is_valid_identifier(name)) {
                    throw ParseError{"Line " + std::to_string(lineno) + ": Invalid import name '" + name + "'."};
                }
                imported_names[name] = module_name;
            }
            if (!has_name) {
                throw ParseError{"Line " + std::to_string(lineno) + ": No imports listed."};
            }
            continue;
        }

        if (stripped.rfind("if ", 0) == 0 && stripped.back() == ':') {
            std::string expr = stripped.substr(3, stripped.size() - 4);
            expr.erase(expr.find_last_not_of(' ') + 1);
            out.push_back("if (" + normalize_expression(expr, imports, imported_names) + ") {");
            expect_indent = true;
            continue;
        }

        if (stripped.rfind("while ", 0) == 0 && stripped.back() == ':') {
            std::string expr = stripped.substr(6, stripped.size() - 7);
            expr.erase(expr.find_last_not_of(' ') + 1);
            out.push_back("while (" + normalize_expression(expr, imports, imported_names) + ") {");
            expect_indent = true;
            continue;
        }

        if (stripped == "else:") {
            out.push_back("else {");
            expect_indent = true;
            continue;
        }

        if (stripped.rfind("print(", 0) == 0 && stripped.back() == ')') {
            std::string expr = stripped.substr(6, stripped.size() - 7);
            out.push_back("std::cout << " + normalize_expression(expr, imports, imported_names) + " << std::endl;");
            continue;
        }

        auto assignment = split_assignment(stripped);
        if (!assignment.first.empty() || !assignment.second.empty()) {
            std::string name = assignment.first;
            std::string expr = assignment.second;
            name.erase(0, name.find_first_not_of(' '));
            name.erase(name.find_last_not_of(' ') + 1);
            expr.erase(0, expr.find_first_not_of(' '));
            expr.erase(expr.find_last_not_of(' ') + 1);
            if (!is_valid_identifier(name)) {
                throw ParseError{"Line " + std::to_string(lineno) + ": Invalid variable name."};
            }
            if (defined.find(name) == defined.end()) {
                defined.insert(name);
                out.push_back("auto " + name + " = " + normalize_expression(expr, imports, imported_names) + ";");
            } else {
                out.push_back(name + " = " + normalize_expression(expr, imports, imported_names) + ";");
            }
            continue;
        }

        out.push_back(normalize_expression(stripped, imports, imported_names) + ";");
    }

    while (indent_stack.size() > 1) {
        out.push_back("}");
        indent_stack.pop_back();
    }

    return {out, imports};
}

bool write_cpp(const fs::path& output_path, const std::vector<std::string>& body_lines, const std::vector<std::string>& imports) {
    std::unordered_map<std::string, std::string> library_headers = {
        {"BIFMath", "libs/BIFMath/BIFMath.h"},
        {"BIFitertools", "libs/BIFitertools/BIFitertools.h"},
        {"BIFtkinter", "libs/BIFtkinter/BIFtkinter.h"},
    };

    std::unordered_map<std::string, std::string> using_lines = {
        {"BIFMath", "using bif::math::BIFMath;"},
        {"BIFitertools", "using bif::itertools::BIFitertools;"},
        {"BIFtkinter", "using bif::tkinter::BIFWindow;"},
    };

    std::vector<std::string> content = {
        "#include <iostream>",
        "#include <string>",
        "",
    };

    for (const auto& module_name : imports) {
        auto it = library_headers.find(module_name);
        if (it != library_headers.end()) {
            content.push_back("#include \"" + it->second + "\"");
        }
    }

    content.push_back("");
    for (const auto& module_name : imports) {
        auto it = using_lines.find(module_name);
        if (it != using_lines.end()) {
            content.push_back(it->second);
        }
    }

    content.insert(
        content.end(),
        {
            "",
            "std::string bif_input(const std::string& prompt) {",
            "    if (!prompt.empty()) {",
            "        std::cout << prompt;",
            "    }",
            "    std::string value;",
            "    std::getline(std::cin, value);",
            "    return value;",
            "}",
            "",
            "int main() {",
        });

    for (const auto& line : body_lines) {
        content.push_back("    " + line);
    }
    content.push_back("    return 0;");
    content.push_back("}");

    std::ostringstream joined;
    for (size_t i = 0; i < content.size(); ++i) {
        joined << content[i] << "\n";
    }
    std::string new_content = joined.str();

    if (fs::exists(output_path)) {
        std::ifstream in(output_path, std::ios::binary);
        std::ostringstream current;
        current << in.rdbuf();
        if (current.str() == new_content) {
            return false;
        }
    }

    std::ofstream out(output_path, std::ios::binary);
    out << new_content;
    return true;
}

std::string quote_arg(const std::string& value) {
    if (value.find(' ') == std::string::npos) {
        return value;
    }
    return "\"" + value + "\"";
}

int compile_cpp(const fs::path& cpp_path, const fs::path& exe_path, const fs::path& include_dir) {
    std::string command =
        "g++ -std=c++17 -O2 " + quote_arg(cpp_path.string()) +
        " -I " + quote_arg(include_dir.string()) +
        " -o " + quote_arg(exe_path.string());

    return std::system(command.c_str());
}

int run_exe(const fs::path& exe_path) {
    std::string command = quote_arg(exe_path.string());
    return std::system(command.c_str());
}

std::string read_file_text(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return "";
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    std::string input_path;
    std::string outdir = "build";
    bool run = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--run") {
            run = true;
        } else if (arg == "--outdir") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --outdir" << std::endl;
                return 1;
            }
            outdir = argv[++i];
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        } else if (input_path.empty()) {
            input_path = arg;
        } else {
            std::cerr << "Unexpected argument: " << arg << std::endl;
            return 1;
        }
    }

    if (input_path.empty()) {
        std::cerr << "Input file not found." << std::endl;
        return 1;
    }

    fs::path input = fs::absolute(input_path);
    if (!fs::exists(input)) {
        std::cerr << "Input file not found." << std::endl;
        return 1;
    }

    std::ifstream in(input);
    if (!in) {
        std::cerr << "Input file not found." << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        lines.push_back(line + "\n");
    }

    TranspileResult result;
    try {
        result = transpile_bif(lines);
    } catch (const ParseError& err) {
        std::cerr << err.message << std::endl;
        return 2;
    }

    fs::path outdir_path = fs::absolute(outdir);
    fs::create_directories(outdir_path);

    std::string base_name = input.stem().string();
    fs::path cpp_path = outdir_path / (base_name + ".cpp");
    fs::path exe_path = outdir_path / (base_name + (".exe"));

    bool cpp_changed = write_cpp(cpp_path, result.body, result.imports);

    fs::path compiler_path = fs::absolute(argv[0]);
    bool exe_up_to_date =
        fs::exists(exe_path) &&
        fs::exists(cpp_path) &&
        fs::last_write_time(exe_path) >= fs::last_write_time(cpp_path) &&
        fs::last_write_time(exe_path) >= fs::last_write_time(compiler_path);

    if (cpp_changed || !exe_up_to_date) {
        fs::path repo_root = compiler_path.parent_path().parent_path();
        int compile_result = compile_cpp(cpp_path, exe_path, repo_root);
        if (compile_result != 0) {
            std::cerr << "Compilation failed." << std::endl;
            return 3;
        }
    }

    if (run) {
        return run_exe(exe_path);
    }

    return 0;
}
