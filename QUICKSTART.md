# Bifithon Quick Start Guide

## Installation

### Prerequisites
- Python 3.6+
- C++ compiler (g++ with C++17 support)
- Tkinter (usually comes with Python)

### Linux/Ubuntu
```bash
sudo apt-get update
sudo apt-get install python3 python3-tk g++
```

### macOS
```bash
brew install python gcc
```

### Windows
1. Install Python from python.org
2. Install MinGW or Visual Studio for C++ compiler

## Running the IDE

```bash
# From the repository root
python3 bifithon/ide/bifithon_ide.py
```

Or use the launcher script:
```bash
./bifithon_launcher.sh
```

## Creating Your First Program

1. Create a file with `.bif` extension (e.g., `hello.bif`)
2. Write your code:

```python
def main():
    print("Hello, Bifithon!")
    
    for i in range(5):
        print(i)
```

3. Compile and run:
   - Using IDE: Press F6
   - Using command line:
     ```bash
     python3 bifithon/compiler/bifithon_compiler.py hello.bif
     g++ -std=c++17 -I bifithon/libraries hello.cpp -o hello
     ./hello
     ```

## Language Features

### Variables
```python
x = 10
name = "Bifithon"
```

### Functions
```python
def add(a, b):
    return a + b

def main():
    result = add(5, 3)
    print(result)
```

### Loops
```python
# For loop
for i in range(10):
    print(i)

# While loop
count = 5
while count > 0:
    print(count)
    count = count - 1
```

### Conditionals
```python
if x > 10:
    print("Greater")
elif x == 10:
    print("Equal")
else:
    print("Less")
```

### Using Libraries

#### bifMath
```python
import bifMath

def main():
    x = bifMath.sqrt(16)
    print(x)
    
    angle = bifMath.PI / 2
    print(bifMath.sin(angle))
```

#### biftools
```python
import biftools

def main():
    for i in range(10):
        print(i)
```

#### bifinter (GUI)
```python
import bifinter

def main():
    window = bifinter.Window("My App", 800, 600)
    
    button = window.addWidget(bifinter.Button, "Click Me", my_callback)
    label = window.addWidget(bifinter.Label, "Hello!")
    
    window.mainloop()
```

## IDE Keyboard Shortcuts

- `Ctrl+N` - New File
- `Ctrl+O` - Open File
- `Ctrl+S` - Save File
- `F5` - Compile
- `F6` - Compile and Run

## Examples

Check the `bifithon/examples/` directory for more examples:
- `example1_math.bif` - Mathematical operations
- `example2_loops.bif` - Loops and iterations
- `example3_functions.bif` - Functions and recursion
- `example4_biftools.bif` - Iterator tools

## Troubleshooting

### Compilation Errors
- Make sure g++ supports C++17 or later: `g++ --version`
- Include library path when compiling: `-I bifithon/libraries`

### IDE Won't Start
- Check if Python 3 is installed: `python3 --version`
- Check if Tkinter is installed: `python3 -m tkinter`

### Import Errors
- Make sure to run commands from the repository root
- Use absolute paths if necessary

## Next Steps

- Explore the library headers in `bifithon/libraries/`
- Try modifying the examples
- Create your own programs
- Contribute to the project!

## Support

For issues and questions, please visit the GitHub repository.
