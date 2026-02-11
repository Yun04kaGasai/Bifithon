# Bifithon

Минимальный прототип компилятора и IDE.

## Компилятор

Запуск компилятора для .bif файла:

```
python tools/bifc.py путь\к\файлу.bif
```

Компиляция и запуск:

```
python tools/bifc.py путь\к\файлу.bif --run
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

Запуск редактора:

```
python ide\bifide.py
```