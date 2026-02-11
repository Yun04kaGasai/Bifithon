# Bifithon Architecture

This document explains the internal architecture of the Bifithon programming language.

## Overview

Bifithon is a transpiler-based programming language that:
1. Takes Python-like syntax as input
2. Converts it to C++ code
3. Compiles the C++ code using g++
4. Produces a native executable

## Components

### 1. Compiler (`bifithon_compiler.py`)

The compiler is the core component that translates Bifithon code to C++.

#### Main Classes

**BifithonCompiler**
- Main compiler class
- Manages the compilation process
- Tracks imports, variables, and functions

#### Key Methods

**compile(source_code: str) -> str**
- Main entry point for compilation
- Adds C++ includes and using directives
- Processes code line by line
- Handles indentation-based brace insertion
- Returns complete C++ code

**process_line(line, all_lines, index) -> (cpp_line, skip)**
- Processes a single line of Bifithon code
- Dispatches to specialized handlers based on line type
- Returns the C++ equivalent and number of lines to skip

**process_function(line, ...)**
- Converts Python `def` to C++ function
- Uses `auto` for return type inference
- Supports the special `main()` function

**process_print(line, indent)**
- Converts `print()` to `cout << ... << endl;`
- Handles multiple arguments
- Preserves string literals
- Respects quotes and parentheses in parsing

**convert_expression(expr)**
- Converts Python expressions to C++ equivalents
- Handles boolean operators (and/or/not -> &&/||/!)
- Converts module.method to namespace::method
- Preserves string literals

#### Indentation Handling

The compiler tracks indentation levels to:
- Insert opening braces `{` when indentation increases
- Insert closing braces `}` when indentation decreases
- Handle special cases like `else` and `elif`

### 2. Libraries

#### bifMath.h
C++ header-only library providing mathematical functions.

**Implementation:**
- Namespace `bifMath`
- Inline functions wrapping C++ standard library
- Constants (PI, E, TAU)
- Mathematical operations (sqrt, pow, sin, cos, etc.)
- Special functions (factorial, gcd)

**Design Decisions:**
- Header-only for easy inclusion
- Inline functions for performance
- Mimics Python's math module API

#### biftools.h
C++ library for iteration and functional programming.

**Implementation:**
- Template-based iterators
- Range class with Python-like syntax
- STL algorithm wrappers (map, filter, zip)
- Generator-like classes (Count, Cycle)

**Design Decisions:**
- Templates for type flexibility
- Iterator protocol compatibility
- Safe default limits for infinite iterators

#### bifinter.h
C++ GUI library (framework placeholder).

**Implementation:**
- Widget hierarchy (Widget base class)
- Common controls (Button, Label, Entry, etc.)
- Window management
- Event handling placeholders

**Design Decisions:**
- Object-oriented design
- Virtual methods for polymorphism
- Placeholder methods for future implementation
- Mimics tkinter's API structure

### 3. IDE (`bifithon_ide.py`)

Python-based IDE using Tkinter.

#### Features

**Editor**
- ScrolledText widget for code editing
- Line number panel
- Undo/redo support
- Basic syntax highlighting setup

**Compilation**
- Calls bifithon_compiler.py as subprocess
- Captures output and errors
- Displays results in output panel

**Execution**
- Compiles C++ output with g++
- Runs executable
- Captures and displays program output

**File Management**
- New/Open/Save operations
- File type filtering (.bif files)
- Title bar updates with filename

## Data Flow

```
┌─────────────────┐
│  .bif source    │
│     file        │
└────────┬────────┘
         │
         v
┌─────────────────┐
│ BifithonCompiler│
│   - Lexical     │
│   - Parsing     │
│   - Generation  │
└────────┬────────┘
         │
         v
┌─────────────────┐
│  .cpp file      │
└────────┬────────┘
         │
         v
┌─────────────────┐
│   g++ compiler  │
│   with library  │
│   includes      │
└────────┬────────┘
         │
         v
┌─────────────────┐
│   executable    │
└─────────────────┘
```

## Language Syntax Mapping

### Function Definitions
```
Bifithon:    def func_name(param1, param2):
C++:         auto func_name(auto param1, auto param2) {
```

### Loops
```
Bifithon:    for i in range(10):
C++:         for (int i = 0; i < 10; i += 1) {

Bifithon:    while condition:
C++:         while (condition) {
```

### Conditionals
```
Bifithon:    if condition:
C++:         if (condition) {

Bifithon:    elif condition:
C++:         } else if (condition) {

Bifithon:    else:
C++:         } else {
```

### Print Statements
```
Bifithon:    print("Hello", x)
C++:         cout << "Hello" << " " << x << endl;
```

### Library Calls
```
Bifithon:    bifMath.sqrt(16)
C++:         bifMath::sqrt(16)
```

## Compilation Process

1. **Lexical Analysis**
   - Read source file line by line
   - Identify line types (function, loop, conditional, etc.)
   - Track indentation levels

2. **Parsing**
   - Extract keywords and parameters
   - Process expressions
   - Handle special cases (print, import, etc.)

3. **Code Generation**
   - Generate C++ includes
   - Convert each line to C++ equivalent
   - Insert braces based on indentation
   - Add namespace using directives

4. **Output**
   - Write complete C++ file
   - Preserve original file name with .cpp extension

## Type System

Bifithon uses C++ `auto` keyword for type inference:
- Variables: `auto x = 10;`
- Function parameters: `auto param`
- Return types: `auto function(...)`

This provides:
- Simplicity (Python-like)
- Type safety (C++ compile-time checking)
- Performance (native C++ execution)

## Error Handling

**Compile-time errors:**
- Caught by g++ compiler
- Displayed in IDE output panel
- Include C++ error messages for debugging

**Runtime errors:**
- Native C++ exceptions
- No additional overhead

## Future Enhancements

Potential improvements:
1. Better type inference and annotation support
2. More sophisticated parsing (AST-based)
3. Enhanced error messages
4. Class support improvements
5. Standard library expansion
6. Actual GUI rendering for bifinter
7. Package management system
8. Debugging support
9. Optimization flags
10. Cross-platform compilation

## Performance Characteristics

**Compilation:**
- Two-pass process (Bifithon→C++, C++→binary)
- Compilation time includes both transpilation and C++ compilation
- Typical: <1s for transpilation, varies for C++ compilation

**Runtime:**
- Native C++ performance
- No interpreter overhead
- Comparable to hand-written C++

## Testing

Examples in `bifithon/examples/`:
- Cover basic language features
- Test library functions
- Serve as documentation
- Verify compiler correctness

Use `make run-examples` to test all examples.

## Contributing

When modifying the compiler:
1. Update relevant example files
2. Test with `make run-examples`
3. Ensure backwards compatibility
4. Update documentation
5. Consider error handling
6. Add comments for complex logic

## License

MIT License - See repository for details
