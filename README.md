# Bifithon

Минимальный прототип компилятора и IDE.

## Компилятор

Запуск компилятора для .bif файла:

```
python tools/bifc.py путь\к\файлу.bif
```

Или соберите C++ версию компилятора:

```
g++ -std=c++17 -O2 tools\bifc.cpp -o tools\bifc.exe
```

После сборки IDE будет автоматически использовать tools\bifc.exe.

Компиляция и запуск:

```
python tools/bifc.py путь\к\файлу.bif --run
```

### Поддерживаемый синтаксис (v1)

- `print(ВЫРАЖЕНИЕ)`
- `input(prompt)` возвращает строку
- присваивание: `x = 5`
- `if ВЫРАЖЕНИЕ:` / `else:`
- `while ВЫРАЖЕНИЕ:`
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

Запуск редактора:

```
python ide\bifide.py
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