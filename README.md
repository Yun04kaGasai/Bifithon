# Bifithon

Minimal compiler and IDE prototype.

## Compiler

Run the compiler on a .bif file:

```
python tools/bifc.py path\to\file.bif
```

Compile and run:

```
python tools/bifc.py path\to\file.bif --run
```

### Supported syntax (v1)

- `print(expr)`
- variable assignment: `x = 5`
- `if expr:` / `else:`
- `while expr:`
- indentation with 4 spaces

## IDE

Launch the GUI editor:

```
python ide\bifide.py
```