import os
import subprocess
import sys
import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox
from tkinter import simpledialog


def get_repo_root():
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def run_compiler(file_path, output_text, status_var, source_text):
    repo_root = get_repo_root()
    compiler_path = os.path.join(repo_root, "tools", "bifc.py")
    compiler_exe = os.path.join(repo_root, "tools", "bifc.exe")

    if not os.path.isfile(compiler_path) and not os.path.isfile(compiler_exe):
        messagebox.showerror("Bifithon", "Компилятор не найден: tools/bifc.py или tools/bifc.exe")
        status_var.set("Компилятор не найден")
        return

    if os.path.isfile(compiler_exe):
        command = [compiler_exe, file_path, "--run"]
    else:
        command = [sys.executable, compiler_path, file_path, "--run"]
    stdin_data = ""
    if "input(" in source_text:
        stdin_data = simpledialog.askstring(
            "Ввод",
            "Введите данные для input() (каждая строка — отдельный ввод):",
        )
        if stdin_data is None:
            status_var.set("Запуск отменен")
            return
    status_var.set("Выполнение...")
    result = subprocess.run(
        command,
        cwd=repo_root,
        input=stdin_data,
        capture_output=True,
        text=True,
    )

    output_text.delete("1.0", tk.END)
    if result.stdout:
        output_text.insert(tk.END, result.stdout)
    if result.stderr:
        output_text.insert(tk.END, result.stderr)
    if result.returncode != 0 and not result.stderr:
        output_text.insert(tk.END, f"Код завершения: {result.returncode}\n")
    status_var.set("Готово" if result.returncode == 0 else "Ошибка")


def main():
    root = tk.Tk()
    root.title("BiIDE")
    root.geometry("900x600")

    current_path = {"value": None}
    status_var = tk.StringVar(value="Готово")

    toolbar = tk.Frame(root)
    toolbar.pack(fill=tk.X)
    status_bar = tk.Label(root, textvariable=status_var, anchor="w")
    status_bar.pack(fill=tk.X, side=tk.BOTTOM)

    main_frame = tk.PanedWindow(root, orient=tk.VERTICAL)
    main_frame.pack(fill=tk.BOTH, expand=True)

    editor = tk.Text(main_frame, wrap=tk.NONE, undo=True)
    output = tk.Text(main_frame, height=10, wrap=tk.NONE)
    output.configure(state=tk.NORMAL)

    main_frame.add(editor)
    main_frame.add(output)

    def open_file():
        path = filedialog.askopenfilename(
            title="Открыть файл Bifithon",
            filetypes=[("Bifithon files", "*.bif"), ("All files", "*.*")],
        )
        if not path:
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title(f"BiIDE - {os.path.basename(path)}")
        status_var.set("Файл открыт")

    def open_example_division():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "division.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Пример не найден: examples/division.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - division.bif")
        status_var.set("Открыт пример")

    def open_example_if_else():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "if_else.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Пример не найден: examples/if_else.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - if_else.bif")
        status_var.set("Открыт пример")

    def open_example_while():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "while_loop.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Пример не найден: examples/while_loop.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - while_loop.bif")
        status_var.set("Открыт пример")

    def open_example_input():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "input_echo.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Пример не найден: examples/input_echo.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - input_echo.bif")
        status_var.set("Открыт пример")

    def open_example_tkinter():
        repo_root = get_repo_root()
        path = os.path.join(repo_root, "examples", "tkinter_demo.bif")
        if not os.path.isfile(path):
            messagebox.showerror("BiIDE", "Пример не найден: examples/tkinter_demo.bif")
            return
        with open(path, "r", encoding="utf-8") as handle:
            editor.delete("1.0", tk.END)
            editor.insert(tk.END, handle.read())
        current_path["value"] = path
        root.title("BiIDE - tkinter_demo.bif")
        status_var.set("Открыт пример")

    def save_file():
        path = current_path["value"]
        if not path:
            return save_file_as()
        with open(path, "w", encoding="utf-8") as handle:
            handle.write(editor.get("1.0", tk.END))
        status_var.set("Сохранено")
        return path

    def save_file_as():
        path = filedialog.asksaveasfilename(
            title="Сохранить файл Bifithon",
            defaultextension=".bif",
            filetypes=[("Bifithon files", "*.bif"), ("All files", "*.*")],
        )
        if not path:
            return None
        current_path["value"] = path
        root.title(f"BiIDE - {os.path.basename(path)}")
        status_var.set("Сохранено")
        return save_file()

    def run_file():
        path = save_file()
        if not path:
            return
        run_compiler(path, output, status_var, editor.get("1.0", tk.END))

    open_button = tk.Button(toolbar, text="Открыть", command=open_file)
    open_button.pack(side=tk.LEFT, padx=6, pady=4)

    run_button = tk.Button(toolbar, text="Запуск", command=run_file)
    run_button.pack(side=tk.LEFT, padx=6, pady=4)

    root.bind("<F5>", lambda event: run_file())

    menu_bar = tk.Menu(root)
    file_menu = tk.Menu(menu_bar, tearoff=0)
    file_menu.add_command(label="Открыть", command=open_file)
    file_menu.add_command(label="Сохранить", command=save_file)
    file_menu.add_command(label="Сохранить как", command=save_file_as)
    file_menu.add_separator()
    file_menu.add_command(label="Пример: Деление", command=open_example_division)
    file_menu.add_command(label="Пример: If/Else", command=open_example_if_else)
    file_menu.add_command(label="Пример: While", command=open_example_while)
    file_menu.add_command(label="Пример: Ввод", command=open_example_input)
    file_menu.add_command(label="Пример: Tkinter", command=open_example_tkinter)
    file_menu.add_separator()
    file_menu.add_command(label="Запуск", command=run_file)
    file_menu.add_separator()
    file_menu.add_command(label="Выход", command=root.quit)

    menu_bar.add_cascade(label="Файл", menu=file_menu)
    root.config(menu=menu_bar)

    root.mainloop()


if __name__ == "__main__":
    main()
