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

        if stripped.startswith("if ") and stripped.endswith(":"):
            expr = stripped[3:-1].strip()
            out.append(f"if ({replace_keywords(expr)}) {{")
            expect_indent = True
            continue

        if stripped.startswith("while ") and stripped.endswith(":"):
            expr = stripped[6:-1].strip()
            out.append(f"while ({replace_keywords(expr)}) {{")
            expect_indent = True
            continue

        if stripped == "else:":
            out.append("else {")
            expect_indent = True
            continue

        if stripped.startswith("print(") and stripped.endswith(")"):
            expr = stripped[6:-1].strip()
            out.append(f"std::cout << {replace_keywords(expr)} << std::endl;")
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
                out.append(f"auto {name} = {replace_keywords(expr)};")
            else:
                out.append(f"{name} = {replace_keywords(expr)};")
            continue

        out.append(f"{replace_keywords(stripped)};")

    while len(indent_stack) > 1:
        out.append("}")
        indent_stack.pop()

    return out


def write_cpp(output_path: str, body_lines):
    content = [
        "#include <iostream>",
        "#include <string>",
        "",
        "int main() {",
    ]
    content.extend([f"    {line}" for line in body_lines])
    content.append("    return 0;")
    content.append("}")

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write("\n".join(content))
        handle.write("\n")


def compile_cpp(cpp_path: str, exe_path: str):
    if shutil.which("g++") is None:
        raise RuntimeError("g++ was not found in PATH.")
    command = ["g++", "-std=c++17", "-O2", cpp_path, "-o", exe_path]
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
        body = transpile_bif(lines)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2

    os.makedirs(args.outdir, exist_ok=True)
    base_name = os.path.splitext(os.path.basename(input_path))[0]
    cpp_path = os.path.join(args.outdir, f"{base_name}.cpp")
    exe_name = f"{base_name}.exe" if os.name == "nt" else base_name
    exe_path = os.path.join(args.outdir, exe_name)

    write_cpp(cpp_path, body)

    try:
        compile_cpp(cpp_path, exe_path)
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
