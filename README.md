# Bifithon

Минимальный прототип компилятора и IDE.

## Компилятор

Сборка C++ версии компилятора:

```
g++ -std=c++17 -O2 tools\bifc.cpp -o tools\bifc.exe
```

Linux:

```
g++ -std=c++17 -O2 tools/bifc.cpp -o tools/bifc
```

Компиляция и запуск:

```
tools\bifc.exe путь\к\файлу.bif --run
```

Linux:

```
./tools/bifc путь/к/файлу.bif --run
```

### Поддерживаемый синтаксис (v1)

- `print(expr)`
- `input(prompt)` возвращает строку
- присваивание: `x = 5`
- `if expr:` / `else:`
- `while expr:`
- отступы 4 пробела
- логические ключевые слова: `and`, `or`, `not`
- деление всегда с плавающей точкой при `/`
- `import BIFMath`, `import BIFitertools`, `import BIFtkinter`
- `from BIFMath import sqrt` (и другие функции)
- доступ через модуль: `BIFMath.sqrt(9)` становится `BIFMath::sqrt(9)`

## Библиотеки

Доступные BIF библиотеки:

- BIFMath
- BIFitertools
- BIFtkinter

### BIFitertools

- `range(stop)` / `range(start, stop, step)`
- `repeat(value, times)`
- `count(start, step, count)`
- `cycle(items, times)`
- `chain(first, second)`

### BIFtkinter

- `BIFWindow(title, width, height)`
- `show()` показывает окно
- `wait_for_click()` ждет нажатия кнопки и возвращает `control_id`
- `add_label(text, x, y, width, height)`
- `add_button(text, x, y, width, height)`
- `add_input(x, y, width, height)`
- `get_input_text(control_id)`

## IDE

Сборка IDE (wxWidgets):

```
cmake -S ide -B ide\build
cmake --build ide\build
```

Запуск редактора:

```
ide\build\bifide.exe
```

Linux:

```
ide/build/bifide
```

## Установка

Скрипты установки запускаются из корня репозитория.

### Windows

```
\install.ps1
```

### Linux (Debian/Arch)

```
chmod +x install.sh
./install.sh
```