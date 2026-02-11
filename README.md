# Bifithon

Язык программирования на основе C++ с синтаксисом как в Python

## Описание

Bifithon - это язык программирования, который использует Python-подобный синтаксис и компилируется в C++ для высокой производительности. Язык включает в себя три основные библиотеки и собственную IDE для разработки.

## Основные компоненты

### 1. Библиотеки

#### bifMath
Аналог модуля Math из Python, предоставляет математические функции:
- Базовые операции: `sqrt()`, `pow()`, `abs()`, `ceil()`, `floor()`, `round()`
- Тригонометрия: `sin()`, `cos()`, `tan()`, `asin()`, `acos()`, `atan()`
- Логарифмы: `log()`, `log10()`, `log2()`, `exp()`
- Константы: `PI`, `E`, `TAU`
- Дополнительные функции: `factorial()`, `gcd()`, `degrees()`, `radians()`

#### biftools
Аналог модуля itertools из Python, предоставляет инструменты для работы с итераторами:
- `Range`: итератор для генерации последовательности чисел
- `Count`: бесконечный счетчик
- `Cycle`: циклический итератор
- `combinations()`: генерация комбинаций
- `permutations()`: генерация перестановок
- `filter()`: фильтрация элементов
- `map()`: применение функции к элементам
- `zip()`: объединение последовательностей

#### bifinter
Аналог tkinter из Python, библиотека для создания GUI:
- `Window`: главное окно приложения
- `Button`: кнопка
- `Label`: текстовая метка
- `Entry`: поле ввода текста
- `Text`: многострочный текстовый виджет
- `Frame`: контейнер для виджетов
- `Canvas`: холст для рисования
- `messagebox`: диалоговые окна

### 2. Компилятор

Python-скрипт `bifithon_compiler.py`, который преобразует код на Bifithon в C++:
- Поддержка Python-подобного синтаксиса
- Автоматическое определение типов
- Преобразование циклов и условных операторов
- Интеграция с библиотеками bifMath, biftools, bifinter

### 3. IDE

Графическая среда разработки `bifithon_ide.py` на Python с Tkinter:
- Редактор кода с подсветкой синтаксиса
- Нумерация строк
- Компиляция кода (F5)
- Компиляция и запуск (F6)
- Удобное меню и панель инструментов

## Установка

### Требования
- Python 3.6 или выше
- C++ компилятор (g++ с поддержкой C++17)
- Tkinter для Python (обычно уже установлен)

### Установка зависимостей

```bash
# На Ubuntu/Debian
sudo apt-get install python3 python3-tk g++

# На macOS (через Homebrew)
brew install python gcc
```

## Использование

### Запуск IDE

```bash
python3 bifithon/ide/bifithon_ide.py
```

### Компиляция из командной строки

```bash
# Компиляция .bif файла в .cpp
python3 bifithon/compiler/bifithon_compiler.py your_program.bif

# Компиляция .cpp в исполняемый файл
g++ -std=c++17 -I bifithon/libraries your_program.cpp -o your_program

# Запуск программы
./your_program
```

## Примеры кода

### Пример 1: Математические операции

```python
import bifMath

def main():
    x = bifMath.sqrt(16)
    print("Square root of 16:", x)
    
    angle = bifMath.PI / 4
    print("Sin of 45 degrees:", bifMath.sin(angle))
    
    print("Factorial of 5:", bifMath.factorial(5))
```

### Пример 2: Циклы

```python
def main():
    # Цикл for с range
    for i in range(10):
        print(i)
    
    # Цикл while
    count = 5
    while count > 0:
        print(count)
        count = count - 1
```

### Пример 3: Функции

```python
def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)

def main():
    for i in range(10):
        print(fibonacci(i))
```

## Структура проекта

```
Bifithon/
├── bifithon/
│   ├── compiler/
│   │   └── bifithon_compiler.py    # Компилятор Bifithon -> C++
│   ├── ide/
│   │   └── bifithon_ide.py         # Графическая IDE
│   ├── libraries/
│   │   ├── bifMath.h               # Математическая библиотека
│   │   ├── biftools.h              # Библиотека итераторов
│   │   └── bifinter.h              # GUI библиотека
│   └── examples/
│       ├── example1_math.bif       # Примеры использования
│       ├── example2_loops.bif
│       ├── example3_functions.bif
│       └── example4_biftools.bif
└── README.md
```

## Поддерживаемые возможности

- ✅ Функции (`def`)
- ✅ Классы (`class`)
- ✅ Условные операторы (`if`, `elif`, `else`)
- ✅ Циклы (`for`, `while`)
- ✅ Переменные с автоопределением типов
- ✅ Операторы печати (`print`)
- ✅ Импорт библиотек (`import`)
- ✅ Математические операции (bifMath)
- ✅ Инструменты итераторов (biftools)
- ✅ GUI компоненты (bifinter)

## Горячие клавиши в IDE

- `Ctrl+N` - Новый файл
- `Ctrl+O` - Открыть файл
- `Ctrl+S` - Сохранить файл
- `F5` - Компилировать
- `F6` - Компилировать и запустить

## Разработка

Bifithon находится в активной разработке. Вклад в проект приветствуется!

## Лицензия

MIT License

## Автор

Bifithon Programming Language Project