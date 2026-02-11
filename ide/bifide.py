import os
import subprocess
import sys
import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox


def get_repo_root():
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def run_compiler(file_path, output_text):
    repo_root = get_repo_root()
    compiler_path = os.path.join(repo_root, "tools", "bifc.py")

    if not os.path.isfile(compiler_path):
        messagebox.showerror("Bifithon", "Compiler not found: tools/bifc.py")
        return

    command = [sys.executable, compiler_path, file_path, "--run"]
    result = subprocess.run(
        command,
        cwd=repo_root,
        capture_output=True,
        text=True,
    )

    output_text.delete("1.0", tk.END)
    if result.stdout:
        output_text.insert(tk.END, result.stdout)
    if result.stderr:
        output_text.insert(tk.END, result.stderr)
    if result.returncode != 0 and not result.stderr:
        output_text.insert(tk.END, f"Exit code: {result.returncode}\n")


def main():
    root = tk.Tk()
    root.title("BiIDE")
    root.geometry("900x600")

    current_path = {"value": None}

    main_frame = tk.PanedWindow(root, orient=tk.VERTICAL)
    main_frame.pack(fill=tk.BOTH, expand=True)

    editor = tk.Text(main_frame, wrap=tk.NONE, undo=True)
    output = tk.Text(main_frame, height=10, wrap=tk.NONE)
    output.configure(state=tk.NORMAL)

    main_frame.add(editor)
    main_frame.add(output)

    def open_file():
        path = filedialog.askopenfilename(
            title="Open Bifithon file",
            filetypes=[("Bifithon files", "*.bif"), ("All files", "*.*")],
        )
        if not path:
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title(f"BiIDE - {os.path.basename(path)}")

    def open_example_division():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "division.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Example not found: examples/division.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - division.bif")

    def open_example_if_else():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "if_else.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Example not found: examples/if_else.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - if_else.bif")

    def open_example_while():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "while_loop.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Example not found: examples/while_loop.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - while_loop.bif")

    def save_file():
        path = current_path["value"]
        if not path:
            return save_file_as()
        with open(path, "w", encoding="utf-8") as handle:
            handle.write(editor.get("1.0", tk.END))
        return path

    def save_file_as():
        path = filedialog.asksaveasfilename(
            title="Save Bifithon file",
            defaultextension=".bif",
            filetypes=[("Bifithon files", "*.bif"), ("All files", "*.*")],
        )
        if not path:
            return None
        current_path["value"] = path
        root.title(f"BiIDE - {os.path.basename(path)}")
        return save_file()

    def run_file():
        path = save_file()
        if not path:
            return
        run_compiler(path, output)

    menu_bar = tk.Menu(root)
    file_menu = tk.Menu(menu_bar, tearoff=0)
    file_menu.add_command(label="Open", command=open_file)
    file_menu.add_command(label="Save", command=save_file)
    file_menu.add_command(label="Save As", command=save_file_as)
    file_menu.add_separator()
    file_menu.add_command(label="Open Example: Division", command=open_example_division)
    file_menu.add_command(label="Open Example: If/Else", command=open_example_if_else)
    file_menu.add_command(label="Open Example: While", command=open_example_while)
    file_menu.add_separator()
    file_menu.add_command(label="Run", command=run_file)
    file_menu.add_separator()
    file_menu.add_command(label="Exit", command=root.quit)

    menu_bar.add_cascade(label="File", menu=file_menu)
    root.config(menu=menu_bar)

    root.mainloop()


if __name__ == "__main__":
    main()
