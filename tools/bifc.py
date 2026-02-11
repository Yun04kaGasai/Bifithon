import argparse
import os
import shutil
import subprocess
import sys


def strip_comment(line: str) -> str:
    in_string = False
    string_char = ""
    escaped = False
    out = []
    for ch in line:
        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            continue

        if ch in ("\"", "'"):
            in_string = True
            string_char = ch
            out.append(ch)
            continue

        if ch == "#":
            break

        out.append(ch)

    return "".join(out)


def replace_keywords(expr: str) -> str:
    replacements = {
        "and": "&&",
        "or": "||",
        "not": "!",
        "True": "true",
        "False": "false",
        "None": "nullptr",
    }

    out = []
    word = ""
    in_string = False
    string_char = ""
    escaped = False

    def flush_word():
        nonlocal word
        if word:
            out.append(replacements.get(word, word))
            word = ""

    for ch in expr:
        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            continue

        if ch in ("\"", "'"):
            flush_word()
            in_string = True
            string_char = ch
            out.append(ch)
            continue

        if ch.isalnum() or ch == "_":
            word += ch
        else:
            flush_word()
            out.append(ch)

    flush_word()
    return "".join(out)


def expr_has_division(expr: str) -> bool:
    in_string = False
    string_char = ""
    escaped = False

    for ch in expr:
        if in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            continue

        if ch in ("\"", "'"):
            in_string = True
            string_char = ch
            continue

        if ch == "/":
            return True

    return False


def promote_int_literals_for_division(expr: str) -> str:
    out = []
    in_string = False
    string_char = ""
    escaped = False
    i = 0

    while i < len(expr):
        ch = expr[i]

        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            i += 1
            continue

        if ch in ("\"", "'"):
            in_string = True
            string_char = ch
            out.append(ch)
            i += 1
            continue

        if ch.isdigit() and (i == 0 or not (expr[i - 1].isalnum() or expr[i - 1] == "_")):
            start = i
            while i < len(expr) and expr[i].isdigit():
                i += 1

            is_float = False
            if i < len(expr) and expr[i] == ".":
                is_float = True
                i += 1
                while i < len(expr) and expr[i].isdigit():
                    i += 1

            if i < len(expr) and expr[i] in ("e", "E"):
                is_float = True
                i += 1
                if i < len(expr) and expr[i] in ("+", "-"):
                    i += 1
                while i < len(expr) and expr[i].isdigit():
                    i += 1

            token = expr[start:i]
            if not is_float:
                token += ".0"
            out.append(token)
            continue

        out.append(ch)
        i += 1

    return "".join(out)


def replace_input_calls(expr: str) -> str:
    out = []
    in_string = False
    string_char = ""
    escaped = False
    i = 0

    while i < len(expr):
        ch = expr[i]

        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            i += 1
            continue

        if ch in ("\"", "'"):
            in_string = True
            string_char = ch
            out.append(ch)
            i += 1
            continue

        if expr.startswith("input(", i):
            out.append("bif_input(")
            i += len("input(")
            continue

        out.append(ch)
        i += 1

    return "".join(out)


def replace_module_access(expr: str, modules) -> str:
    out = []
    in_string = False
    string_char = ""
    escaped = False
    i = 0

    while i < len(expr):
        ch = expr[i]

        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            i += 1
            continue

        if ch in ("\"", "'"):
            in_string = True
            string_char = ch
            out.append(ch)
            i += 1
            continue

        matched = False
        for module_name in modules:
            token = f"{module_name}."
            if expr.startswith(token, i):
                out.append(f"{module_name}::")
                i += len(token)
                matched = True
                break

        if matched:
            continue

        out.append(ch)
        i += 1

    return "".join(out)


def replace_imported_names(expr: str, imported_names) -> str:
    out = []
    word = ""
    in_string = False
    string_char = ""
    escaped = False

    def flush_word():
        nonlocal word
        if not word:
            return
        module_name = imported_names.get(word)
        if module_name:
            out.append(f"{module_name}::{word}")
        else:
            out.append(word)
        word = ""

    for ch in expr:
        if in_string:
            out.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == string_char:
                in_string = False
            continue

        if ch in ("\"", "'"):
            flush_word()
            in_string = True
            string_char = ch
            out.append(ch)
            continue

        if ch.isalnum() or ch == "_":
            word += ch
        else:
            flush_word()
            out.append(ch)

    flush_word()
    return "".join(out)


def normalize_expression(expr: str, modules=None, imported_names=None) -> str:
    expr = replace_keywords(expr)
    expr = replace_input_calls(expr)
    if modules:
        expr = replace_module_access(expr, modules)
    if imported_names:
        expr = replace_imported_names(expr, imported_names)
    if expr_has_division(expr):
        expr = promote_int_literals_for_division(expr)
    return expr


def split_assignment(line: str):
    for i, ch in enumerate(line):
        if ch != "=":
            continue
        prev = line[i - 1] if i > 0 else ""
        nxt = line[i + 1] if i + 1 < len(line) else ""
        if prev in ("=", "!", "<", ">") or nxt == "=":
            continue
        return line[:i].strip(), line[i + 1 :].strip()
    return None


def transpile_bif(lines):
    out = []
    indent_stack = [0]
    expect_indent = False
    defined = set()
    imports = []
    known_imports = {"BIFMath", "BIFitertools", "BIFtkinter"}
    imported_names = {}

    for lineno, raw in enumerate(lines, 1):
        line = strip_comment(raw.rstrip("\n"))
        if not line.strip():
            continue
        if "\t" in line:
            raise ValueError(f"Line {lineno}: Tabs are not allowed. Use 4 spaces.")

        indent = len(line) - len(line.lstrip(" "))
        if indent % 4 != 0:
            raise ValueError(f"Line {lineno}: Indentation must be multiples of 4 spaces.")

        if indent > indent_stack[-1]:
            if not expect_indent:
                raise ValueError(f"Line {lineno}: Unexpected indentation.")
            indent_stack.append(indent)
            expect_indent = False

        while indent < indent_stack[-1]:
            out.append("}")
            indent_stack.pop()

        if expect_indent and indent == indent_stack[-1]:
            raise ValueError(f"Line {lineno}: Expected indented block.")

        stripped = line.strip()

        if stripped.startswith("import "):
            module_name = stripped[len("import ") :].strip()
            if module_name not in known_imports:
                raise ValueError(f"Line {lineno}: Unknown module '{module_name}'.")
            if module_name not in imports:
                imports.append(module_name)
            continue

        if stripped.startswith("from ") and " import " in stripped:
            before, after = stripped.split(" import ", 1)
            module_name = before[len("from ") :].strip()
            if module_name not in known_imports:
                raise ValueError(f"Line {lineno}: Unknown module '{module_name}'.")
            if module_name not in imports:
                imports.append(module_name)
            names = [name.strip() for name in after.split(",") if name.strip()]
            if not names:
                raise ValueError(f"Line {lineno}: No imports listed.")
            for name in names:
                if not all(ch.isalnum() or ch == "_" for ch in name):
                    raise ValueError(f"Line {lineno}: Invalid import name '{name}'.")
                imported_names[name] = module_name
            continue

        if stripped.startswith("if ") and stripped.endswith(":"):
            expr = stripped[3:-1].strip()
            out.append(f"if ({normalize_expression(expr, imports, imported_names)}) {{")
            expect_indent = True
            continue

        if stripped.startswith("while ") and stripped.endswith(":"):
            expr = stripped[6:-1].strip()
            out.append(f"while ({normalize_expression(expr, imports, imported_names)}) {{")
            expect_indent = True
            continue

        if stripped == "else:":
            out.append("else {")
            expect_indent = True
            continue

        if stripped.startswith("print(") and stripped.endswith(")"):
            expr = stripped[6:-1].strip()
            out.append(f"std::cout << {normalize_expression(expr, imports, imported_names)} << std::endl;")
            continue

        assignment = split_assignment(stripped)
        if assignment:
            name, expr = assignment
            if not name or not (name[0].isalpha() or name[0] == "_"):
                raise ValueError(f"Line {lineno}: Invalid variable name.")
            if not all(ch.isalnum() or ch == "_" for ch in name):
                raise ValueError(f"Line {lineno}: Invalid variable name.")
            if name not in defined:
                defined.add(name)
                out.append(f"auto {name} = {normalize_expression(expr, imports, imported_names)};")
            else:
                out.append(f"{name} = {normalize_expression(expr, imports, imported_names)};")
            continue

        out.append(f"{normalize_expression(stripped, imports, imported_names)};")

    while len(indent_stack) > 1:
        out.append("}")
        indent_stack.pop()

    return out, imports


def write_cpp(output_path: str, body_lines, imports):
    library_headers = {
        "BIFMath": "libs/BIFMath/BIFMath.h",
        "BIFitertools": "libs/BIFitertools/BIFitertools.h",
        "BIFtkinter": "libs/BIFtkinter/BIFtkinter.h",
    }

    using_lines = {
        "BIFMath": "using bif::math::BIFMath;",
        "BIFitertools": "using bif::itertools::BIFitertools;",
        "BIFtkinter": "using bif::tkinter::BIFWindow;",
    }

    content = [
        "#include <iostream>",
        "#include <string>",
        "",
    ]

    for module_name in imports:
        header = library_headers.get(module_name)
        if header:
            content.append(f"#include \"{header}\"")

    content.append("")
    for module_name in imports:
        using_line = using_lines.get(module_name)
        if using_line:
            content.append(using_line)

    content.extend(
        [
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
        ]
    )
    content.extend([f"    {line}" for line in body_lines])
    content.append("    return 0;")
    content.append("}")

    new_content = "\n".join(content) + "\n"

    if os.path.isfile(output_path):
        with open(output_path, "r", encoding="utf-8") as handle:
            if handle.read() == new_content:
                return False

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write(new_content)
    return True


def compile_cpp(cpp_path: str, exe_path: str, include_dir: str):
    if shutil.which("g++") is None:
        raise RuntimeError("g++ was not found in PATH.")
    command = ["g++", "-std=c++17", "-O2", cpp_path, "-I", include_dir, "-o", exe_path]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "Compilation failed.")


def run_exe(exe_path: str):
    result = subprocess.run([exe_path], capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr


def main():
    parser = argparse.ArgumentParser(description="Bifithon compiler")
    parser.add_argument("input", help="Input .bif file")
    parser.add_argument("--run", action="store_true", help="Run after compile")
    parser.add_argument("--outdir", default="build", help="Output directory")
    args = parser.parse_args()

    input_path = os.path.abspath(args.input)
    if not os.path.isfile(input_path):
        print("Input file not found.", file=sys.stderr)
        return 1

    with open(input_path, "r", encoding="utf-8") as handle:
        lines = handle.readlines()

    try:
        body, imports = transpile_bif(lines)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2

    os.makedirs(args.outdir, exist_ok=True)
    base_name = os.path.splitext(os.path.basename(input_path))[0]
    cpp_path = os.path.join(args.outdir, f"{base_name}.cpp")
    exe_name = f"{base_name}.exe" if os.name == "nt" else base_name
    exe_path = os.path.join(args.outdir, exe_name)

    cpp_changed = write_cpp(cpp_path, body, imports)

    compiler_mtime = os.path.getmtime(__file__)
    exe_up_to_date = (
        os.path.isfile(exe_path)
        and os.path.isfile(cpp_path)
        and os.path.getmtime(exe_path) >= os.path.getmtime(cpp_path)
        and os.path.getmtime(exe_path) >= compiler_mtime
    )

    try:
        if cpp_changed or not exe_up_to_date:
            repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            compile_cpp(cpp_path, exe_path, repo_root)
    except RuntimeError as exc:
        print(str(exc), file=sys.stderr)
        return 3

    if args.run:
        code, out, err = run_exe(exe_path)
        if out:
            print(out, end="")
        if err:
            print(err, file=sys.stderr, end="")
        return code

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
