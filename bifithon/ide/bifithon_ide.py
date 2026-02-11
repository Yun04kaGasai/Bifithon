#!/usr/bin/env python3
"""
Bifithon IDE
A simple IDE for writing and compiling Bifithon code
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, scrolledtext
import subprocess
import os
import sys


class BifithonIDE:
    def __init__(self, root):
        self.root = root
        self.root.title("Bifithon IDE")
        self.root.geometry("1000x700")
        
        self.current_file = None
        self.compiler_path = os.path.join(
            os.path.dirname(__file__), 
            '..', 'compiler', 'bifithon_compiler.py'
        )
        
        self.create_menu()
        self.create_toolbar()
        self.create_editor()
        self.create_output_panel()
        self.create_statusbar()
        
    def create_menu(self):
        """Create menu bar"""
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # File menu
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Open", command=self.open_file, accelerator="Ctrl+O")
        file_menu.add_command(label="Save", command=self.save_file, accelerator="Ctrl+S")
        file_menu.add_command(label="Save As", command=self.save_as_file)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)
        
        # Edit menu
        edit_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Edit", menu=edit_menu)
        edit_menu.add_command(label="Undo", command=lambda: self.editor.edit_undo())
        edit_menu.add_command(label="Redo", command=lambda: self.editor.edit_redo())
        edit_menu.add_separator()
        edit_menu.add_command(label="Cut", command=lambda: self.editor.event_generate("<<Cut>>"))
        edit_menu.add_command(label="Copy", command=lambda: self.editor.event_generate("<<Copy>>"))
        edit_menu.add_command(label="Paste", command=lambda: self.editor.event_generate("<<Paste>>"))
        
        # Run menu
        run_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Run", menu=run_menu)
        run_menu.add_command(label="Compile", command=self.compile_code, accelerator="F5")
        run_menu.add_command(label="Compile and Run", command=self.compile_and_run, accelerator="F6")
        
        # Help menu
        help_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Help", menu=help_menu)
        help_menu.add_command(label="About", command=self.show_about)
        help_menu.add_command(label="Documentation", command=self.show_docs)
        
        # Bind keyboard shortcuts
        self.root.bind('<Control-n>', lambda e: self.new_file())
        self.root.bind('<Control-o>', lambda e: self.open_file())
        self.root.bind('<Control-s>', lambda e: self.save_file())
        self.root.bind('<F5>', lambda e: self.compile_code())
        self.root.bind('<F6>', lambda e: self.compile_and_run())
        
    def create_toolbar(self):
        """Create toolbar"""
        toolbar = ttk.Frame(self.root)
        toolbar.pack(side=tk.TOP, fill=tk.X, padx=2, pady=2)
        
        ttk.Button(toolbar, text="New", command=self.new_file).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Open", command=self.open_file).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Save", command=self.save_file).pack(side=tk.LEFT, padx=2)
        
        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        ttk.Button(toolbar, text="Compile", command=self.compile_code).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Run", command=self.compile_and_run).pack(side=tk.LEFT, padx=2)
        
    def create_editor(self):
        """Create text editor"""
        editor_frame = ttk.Frame(self.root)
        editor_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=2, pady=2)
        
        # Line numbers
        self.line_numbers = tk.Text(editor_frame, width=4, padx=4, takefocus=0,
                                     border=0, background='lightgray', state='disabled')
        self.line_numbers.pack(side=tk.LEFT, fill=tk.Y)
        
        # Editor
        self.editor = scrolledtext.ScrolledText(editor_frame, wrap=tk.NONE, undo=True,
                                                 font=('Courier New', 11))
        self.editor.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Bind events for line numbers
        self.editor.bind('<<Change>>', self.update_line_numbers)
        self.editor.bind('<Configure>', self.update_line_numbers)
        self.editor.bind('<KeyRelease>', self.update_line_numbers)
        
        # Syntax highlighting (basic)
        self.setup_syntax_highlighting()
        
    def create_output_panel(self):
        """Create output panel"""
        output_frame = ttk.LabelFrame(self.root, text="Output", padding=5)
        output_frame.pack(side=tk.BOTTOM, fill=tk.BOTH, expand=False, 
                         padx=2, pady=2, ipady=5)
        
        self.output = scrolledtext.ScrolledText(output_frame, height=10, 
                                                font=('Courier New', 9))
        self.output.pack(fill=tk.BOTH, expand=True)
        
    def create_statusbar(self):
        """Create status bar"""
        self.statusbar = ttk.Label(self.root, text="Ready", relief=tk.SUNKEN)
        self.statusbar.pack(side=tk.BOTTOM, fill=tk.X)
        
    def setup_syntax_highlighting(self):
        """Setup basic syntax highlighting"""
        self.editor.tag_config('keyword', foreground='blue')
        self.editor.tag_config('string', foreground='green')
        self.editor.tag_config('comment', foreground='gray')
        self.editor.tag_config('number', foreground='purple')
        
    def update_line_numbers(self, event=None):
        """Update line numbers"""
        lines = self.editor.get('1.0', tk.END).count('\n')
        line_numbers_string = '\n'.join(str(i) for i in range(1, lines))
        
        self.line_numbers.config(state='normal')
        self.line_numbers.delete('1.0', tk.END)
        self.line_numbers.insert('1.0', line_numbers_string)
        self.line_numbers.config(state='disabled')
        
    def new_file(self):
        """Create new file"""
        if self.editor.get('1.0', tk.END).strip():
            if messagebox.askyesno("New File", "Current file will be lost. Continue?"):
                self.editor.delete('1.0', tk.END)
                self.current_file = None
                self.root.title("Bifithon IDE - Untitled")
        else:
            self.editor.delete('1.0', tk.END)
            self.current_file = None
            self.root.title("Bifithon IDE - Untitled")
        
    def open_file(self):
        """Open file"""
        filename = filedialog.askopenfilename(
            title="Open File",
            filetypes=[("Bifithon Files", "*.bif"), ("All Files", "*.*")]
        )
        
        if filename:
            try:
                with open(filename, 'r') as f:
                    content = f.read()
                self.editor.delete('1.0', tk.END)
                self.editor.insert('1.0', content)
                self.current_file = filename
                self.root.title(f"Bifithon IDE - {os.path.basename(filename)}")
                self.statusbar.config(text=f"Opened: {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open file:\n{e}")
        
    def save_file(self):
        """Save file"""
        if self.current_file:
            try:
                content = self.editor.get('1.0', tk.END)
                with open(self.current_file, 'w') as f:
                    f.write(content)
                self.statusbar.config(text=f"Saved: {self.current_file}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save file:\n{e}")
        else:
            self.save_as_file()
        
    def save_as_file(self):
        """Save file as"""
        filename = filedialog.asksaveasfilename(
            title="Save As",
            defaultextension=".bif",
            filetypes=[("Bifithon Files", "*.bif"), ("All Files", "*.*")]
        )
        
        if filename:
            try:
                content = self.editor.get('1.0', tk.END)
                with open(filename, 'w') as f:
                    f.write(content)
                self.current_file = filename
                self.root.title(f"Bifithon IDE - {os.path.basename(filename)}")
                self.statusbar.config(text=f"Saved: {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save file:\n{e}")
        
    def compile_code(self):
        """Compile current code"""
        # Save first
        if not self.current_file:
            self.save_as_file()
            if not self.current_file:
                return
        else:
            self.save_file()
        
        self.output.delete('1.0', tk.END)
        self.output.insert(tk.END, "Compiling...\n")
        self.statusbar.config(text="Compiling...")
        
        try:
            # Run compiler
            result = subprocess.run(
                [sys.executable, self.compiler_path, self.current_file],
                capture_output=True,
                text=True,
                timeout=30
            )
            
            self.output.insert(tk.END, result.stdout)
            if result.stderr:
                self.output.insert(tk.END, "\nErrors:\n" + result.stderr)
            
            if result.returncode == 0:
                self.output.insert(tk.END, "\nCompilation successful!\n")
                self.statusbar.config(text="Compilation successful")
            else:
                self.output.insert(tk.END, "\nCompilation failed!\n")
                self.statusbar.config(text="Compilation failed")
                
        except Exception as e:
            self.output.insert(tk.END, f"\nError: {e}\n")
            self.statusbar.config(text="Compilation error")
        
    def compile_and_run(self):
        """Compile and run current code"""
        self.compile_code()
        
        if not self.current_file:
            return
        
        cpp_file = self.current_file.rsplit('.', 1)[0] + '.cpp'
        if not os.path.exists(cpp_file):
            messagebox.showerror("Error", "C++ file not found. Compilation may have failed.")
            return
        
        # Compile C++ to executable
        exe_file = self.current_file.rsplit('.', 1)[0]
        if sys.platform == 'win32':
            exe_file += '.exe'
        
        self.output.insert(tk.END, "\nCompiling C++ to executable...\n")
        
        try:
            # Get library path
            lib_path = os.path.join(os.path.dirname(self.compiler_path), 
                                   '..', 'libraries')
            lib_path = os.path.abspath(lib_path)
            
            result = subprocess.run(
                ['g++', '-std=c++17', f'-I{lib_path}', cpp_file, '-o', exe_file],
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0:
                self.output.insert(tk.END, "C++ compilation successful!\n")
                
                # Run executable
                self.output.insert(tk.END, "\nRunning program...\n")
                self.output.insert(tk.END, "=" * 50 + "\n")
                
                run_result = subprocess.run(
                    [exe_file],
                    capture_output=True,
                    text=True,
                    timeout=30
                )
                
                self.output.insert(tk.END, run_result.stdout)
                if run_result.stderr:
                    self.output.insert(tk.END, "\nStderr:\n" + run_result.stderr)
                self.output.insert(tk.END, "\n" + "=" * 50 + "\n")
                self.statusbar.config(text="Program executed")
            else:
                self.output.insert(tk.END, "C++ compilation failed!\n")
                self.output.insert(tk.END, result.stderr)
                self.statusbar.config(text="C++ compilation failed")
                
        except Exception as e:
            self.output.insert(tk.END, f"\nError: {e}\n")
            self.statusbar.config(text="Execution error")
        
    def show_about(self):
        """Show about dialog"""
        messagebox.showinfo(
            "About Bifithon IDE",
            "Bifithon IDE v1.0\n\n"
            "A Python-syntax language that compiles to C++\n\n"
            "Libraries:\n"
            "- bifMath: Math operations\n"
            "- biftools: Iterator tools\n"
            "- bifinter: GUI framework"
        )
        
    def show_docs(self):
        """Show documentation"""
        docs = """
Bifithon Language Documentation
================================

Syntax: Python-like syntax that compiles to C++

Supported Features:
- Functions (def keyword)
- Classes
- If/elif/else statements
- For loops (for x in range(...))
- While loops
- Variables (auto type inference)
- Print statements

Libraries:
- bifMath: Mathematical functions
- biftools: Iterator and functional tools
- bifinter: GUI framework (tkinter-like)

Example:
--------
import bifMath

def main():
    x = bifMath.sqrt(16)
    print(x)
    
    for i in range(10):
        print(i)
"""
        
        # Create a new window for documentation
        doc_window = tk.Toplevel(self.root)
        doc_window.title("Bifithon Documentation")
        doc_window.geometry("600x500")
        
        text = scrolledtext.ScrolledText(doc_window, wrap=tk.WORD, 
                                        font=('Courier New', 10))
        text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        text.insert('1.0', docs)
        text.config(state='disabled')


def main():
    root = tk.Tk()
    ide = BifithonIDE(root)
    root.mainloop()


if __name__ == '__main__':
    main()
