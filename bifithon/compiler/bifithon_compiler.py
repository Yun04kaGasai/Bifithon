#!/usr/bin/env python3
"""
Bifithon Compiler
Transpiles Python-like syntax to C++ code
"""

import re
import os
import sys
from typing import List, Dict, Tuple


class BifithonCompiler:
    def __init__(self):
        self.indent_level = 0
        self.in_function = False
        self.variables = {}
        self.functions = {}
        self.imports = set()
        
    def compile(self, source_code: str) -> str:
        """Compile Bifithon source code to C++"""
        lines = source_code.split('\n')
        cpp_lines = []
        
        # Add includes
        cpp_lines.append('#include <iostream>')
        cpp_lines.append('#include <string>')
        cpp_lines.append('#include <vector>')
        cpp_lines.append('#include <map>')
        
        # Process imports
        for line in lines:
            if line.strip().startswith('import '):
                self.process_import(line)
        
        # Add custom library includes
        for lib in self.imports:
            if lib == 'bifMath':
                cpp_lines.append('#include "bifMath.h"')
            elif lib == 'biftools':
                cpp_lines.append('#include "biftools.h"')
            elif lib == 'bifinter':
                cpp_lines.append('#include "bifinter.h"')
        
        cpp_lines.append('')
        cpp_lines.append('using namespace std;')
        cpp_lines.append('')
        
        # Process code with indentation tracking
        prev_indent = 0
        i = 0
        while i < len(lines):
            line = lines[i]
            current_indent = len(line) - len(line.lstrip())
            
            # Close braces for dedentation
            if line.strip() and not line.strip().startswith('#'):
                # Don't close braces before else/elif
                if not (line.strip().startswith('} else') or line.strip().startswith('elif') or line.strip().startswith('else:')):
                    indent_diff = prev_indent - current_indent
                    if indent_diff > 0:
                        num_closes = indent_diff // 4
                        for j in range(num_closes):
                            cpp_lines.append('    ' * (current_indent // 4 + num_closes - j - 1) + '}')
            
            cpp_line, skip = self.process_line(line, lines, i)
            if cpp_line:
                cpp_lines.append(cpp_line)
                if line.strip():
                    prev_indent = current_indent
                    # Update prev_indent if we opened a block
                    if cpp_line.rstrip().endswith('{'):
                        prev_indent = current_indent + 4
            i += skip + 1
        
        # Close any remaining open braces
        while prev_indent > 0:
            prev_indent -= 4
            cpp_lines.append('    ' * (prev_indent // 4) + '}')
        
        return '\n'.join(cpp_lines)
    
    def process_import(self, line: str):
        """Process import statements"""
        match = re.match(r'import\s+(\w+)', line.strip())
        if match:
            lib = match.group(1)
            self.imports.add(lib)
    
    def process_line(self, line: str, all_lines: List[str], index: int) -> Tuple[str, int]:
        """Process a single line of Bifithon code"""
        stripped = line.strip()
        indent = len(line) - len(line.lstrip())
        
        # Skip empty lines and comments
        if not stripped or stripped.startswith('#'):
            return ('', 0)
        
        # Import statements
        if stripped.startswith('import '):
            return ('', 0)
        
        # Function definition
        if stripped.startswith('def '):
            return self.process_function(line, all_lines, index)
        
        # Class definition
        if stripped.startswith('class '):
            return self.process_class(line, all_lines, index)
        
        # If statement
        if stripped.startswith('if '):
            return self.process_if(line)
        
        # Elif statement
        if stripped.startswith('elif '):
            cpp = '} else if (' + self.convert_expression(stripped[5:].rstrip(':')) + ') {'
            return ('    ' * (indent // 4) + cpp, 0)
        
        # Else statement
        if stripped.startswith('else:'):
            return ('    ' * (indent // 4) + '} else {', 0)
        
        # For loop
        if stripped.startswith('for '):
            return self.process_for(line)
        
        # While loop
        if stripped.startswith('while '):
            return self.process_while(line)
        
        # Return statement
        if stripped.startswith('return '):
            expr = self.convert_expression(stripped[7:])
            return ('    ' * (indent // 4) + 'return ' + expr + ';', 0)
        
        # Print statement
        if stripped.startswith('print('):
            return self.process_print(line, indent)
        
        # Variable assignment
        if '=' in stripped and not any(op in stripped for op in ['==', '!=', '<=', '>=']):
            return self.process_assignment(line, indent)
        
        # Regular statement
        cpp = self.convert_expression(stripped)
        if not cpp.endswith(';') and not cpp.endswith('{') and not cpp.endswith('}'):
            cpp += ';'
        return ('    ' * (indent // 4) + cpp, 0)
    
    def process_function(self, line: str, all_lines: List[str], index: int) -> Tuple[str, int]:
        """Process function definition"""
        match = re.match(r'def\s+(\w+)\((.*?)\):', line.strip())
        if match:
            func_name = match.group(1)
            params = match.group(2)
            
            # Convert parameters
            cpp_params = []
            if params.strip():
                for param in params.split(','):
                    param = param.strip()
                    # Simple type inference - can be improved
                    cpp_params.append('auto ' + param)
            
            # Determine return type (simplified - always auto for now)
            if func_name == 'main':
                cpp = 'int main() {'
            else:
                cpp = 'auto ' + func_name + '(' + ', '.join(cpp_params) + ') {'
            
            indent = len(line) - len(line.lstrip())
            return ('    ' * (indent // 4) + cpp, 0)
        return ('', 0)
    
    def process_class(self, line: str, all_lines: List[str], index: int) -> Tuple[str, int]:
        """Process class definition"""
        match = re.match(r'class\s+(\w+)(?:\((.*?)\))?:', line.strip())
        if match:
            class_name = match.group(1)
            base_class = match.group(2) if match.group(2) else None
            
            indent = len(line) - len(line.lstrip())
            if base_class:
                cpp = f'class {class_name} : public {base_class} {{'
            else:
                cpp = f'class {class_name} {{'
            return ('    ' * (indent // 4) + cpp + '\npublic:', 0)
        return ('', 0)
    
    def process_if(self, line: str) -> Tuple[str, int]:
        """Process if statement"""
        stripped = line.strip()
        condition = stripped[3:].rstrip(':')
        cpp_condition = self.convert_expression(condition)
        indent = len(line) - len(line.lstrip())
        return ('    ' * (indent // 4) + 'if (' + cpp_condition + ') {', 0)
    
    def process_for(self, line: str) -> Tuple[str, int]:
        """Process for loop"""
        stripped = line.strip()
        match = re.match(r'for\s+(\w+)\s+in\s+range\((.*?)\):', stripped)
        if match:
            var = match.group(1)
            range_args = match.group(2)
            args = [arg.strip() for arg in range_args.split(',')]
            
            if len(args) == 1:
                start, end, step = '0', args[0], '1'
            elif len(args) == 2:
                start, end, step = args[0], args[1], '1'
            else:
                start, end, step = args[0], args[1], args[2]
            
            indent = len(line) - len(line.lstrip())
            cpp = f'for (int {var} = {start}; {var} < {end}; {var} += {step}) {{'
            return ('    ' * (indent // 4) + cpp, 0)
        
        # For in container
        match = re.match(r'for\s+(\w+)\s+in\s+(.+?):', stripped)
        if match:
            var = match.group(1)
            container = match.group(2)
            indent = len(line) - len(line.lstrip())
            cpp = f'for (auto {var} : {container}) {{'
            return ('    ' * (indent // 4) + cpp, 0)
        
        return ('', 0)
    
    def process_while(self, line: str) -> Tuple[str, int]:
        """Process while loop"""
        stripped = line.strip()
        condition = stripped[6:].rstrip(':')
        cpp_condition = self.convert_expression(condition)
        indent = len(line) - len(line.lstrip())
        return ('    ' * (indent // 4) + 'while (' + cpp_condition + ') {', 0)
    
    def process_print(self, line: str, indent: int) -> Tuple[str, int]:
        """Process print statement"""
        stripped = line.strip()
        # Extract content between print( and )
        start = stripped.find('print(') + 6
        end = stripped.rfind(')')
        
        if start >= 6 and end > start:
            args_str = stripped[start:end]
            
            # Split by comma but be careful with nested function calls and strings
            args = []
            current_arg = ""
            paren_depth = 0
            in_string = False
            string_char = None
            
            for i, char in enumerate(args_str):
                if char in ('"', "'") and (i == 0 or args_str[i-1] != '\\'):
                    if not in_string:
                        in_string = True
                        string_char = char
                    elif char == string_char:
                        in_string = False
                        string_char = None
                    current_arg += char
                elif char == '(' and not in_string:
                    paren_depth += 1
                    current_arg += char
                elif char == ')' and not in_string:
                    paren_depth -= 1
                    current_arg += char
                elif char == ',' and paren_depth == 0 and not in_string:
                    args.append(current_arg.strip())
                    current_arg = ""
                else:
                    current_arg += char
            
            if current_arg.strip():
                args.append(current_arg.strip())
            
            # Convert each argument
            cpp_args = []
            for arg in args:
                cpp_args.append(self.convert_expression(arg))
            
            cpp = 'cout << ' + ' << " " << '.join(cpp_args) + ' << endl;'
            return ('    ' * (indent // 4) + cpp, 0)
        return ('', 0)
    
    def process_assignment(self, line: str, indent: int) -> Tuple[str, int]:
        """Process variable assignment"""
        stripped = line.strip()
        parts = stripped.split('=', 1)
        var_name = parts[0].strip()
        value = parts[1].strip()
        
        cpp_value = self.convert_expression(value)
        
        # Type inference (simplified)
        if var_name not in self.variables:
            cpp = f'auto {var_name} = {cpp_value};'
            self.variables[var_name] = True
        else:
            cpp = f'{var_name} = {cpp_value};'
        
        return ('    ' * (indent // 4) + cpp, 0)
    
    def convert_expression(self, expr: str) -> str:
        """Convert Python expression to C++"""
        expr = expr.strip()
        
        # Don't convert operators inside strings
        # Simple check: if the expression is a string literal, return as-is
        if (expr.startswith('"') and expr.endswith('"')) or \
           (expr.startswith("'") and expr.endswith("'")):
            return expr
        
        # Boolean operators (only outside of strings)
        expr = expr.replace(' and ', ' && ')
        expr = expr.replace(' or ', ' || ')
        expr = expr.replace(' not ', ' !')
        
        # None to nullptr
        expr = expr.replace('None', 'nullptr')
        
        # True/False
        expr = expr.replace('True', 'true')
        expr = expr.replace('False', 'false')
        
        # Convert module.function to namespace::function for bifithon libraries
        for lib in ['bifMath', 'biftools', 'bifinter']:
            expr = expr.replace(f'{lib}.', f'{lib}::')
        
        # List comprehensions (basic)
        # This is a simplified version
        
        return expr
    
    def add_closing_braces(self, cpp_code: str) -> str:
        """Add missing closing braces based on indentation"""
        lines = cpp_code.split('\n')
        result = []
        prev_indent = 0
        
        for i in range(len(lines)):
            line = lines[i]
            stripped = line.strip()
            
            # Skip empty lines
            if not stripped:
                result.append(line)
                continue
            
            current_indent = len(line) - len(line.lstrip())
            
            # Don't process lines that already start with }
            if stripped.startswith('}'):
                result.append(line)
                prev_indent = current_indent
                continue
            
            # Check next line to see if indent decreases
            next_indent = current_indent
            if i + 1 < len(lines):
                next_line = lines[i + 1]
                if next_line.strip():  # Only if next line is not empty
                    next_indent = len(next_line) - len(next_line.lstrip())
                    next_stripped = next_line.strip()
                    # Don't consider } else as indent decrease
                    if next_stripped.startswith('} else') or next_stripped.startswith('else'):
                        next_indent = current_indent
            
            # Add current line
            result.append(line)
            
            # If next line has less indentation and current line doesn't open a block
            # we might need to close braces
            # But we handle this differently - we close braces when we see the dedent
            
            # If current line opens a block and next line has same or less indent, close it
            if line.rstrip().endswith('{') and next_indent <= current_indent:
                # Check if next line is not part of this block
                if i + 1 < len(lines) and lines[i + 1].strip():
                    next_stripped = lines[i + 1].strip()
                    if not (next_stripped.startswith('} else') or next_stripped.startswith('else')):
                        # Block is empty or has one line, let natural closing handle it
                        pass
            
            prev_indent = current_indent
        
        return '\n'.join(result)


def main():
    if len(sys.argv) < 2:
        print("Usage: bifithon_compiler.py <source_file.bif>")
        sys.exit(1)
    
    source_file = sys.argv[1]
    
    if not os.path.exists(source_file):
        print(f"Error: File '{source_file}' not found")
        sys.exit(1)
    
    with open(source_file, 'r') as f:
        source_code = f.read()
    
    compiler = BifithonCompiler()
    cpp_code = compiler.compile(source_code)
    
    # Output to .cpp file
    output_file = source_file.rsplit('.', 1)[0] + '.cpp'
    with open(output_file, 'w') as f:
        f.write(cpp_code)
    
    print(f"Compiled {source_file} -> {output_file}")


if __name__ == '__main__':
    main()
