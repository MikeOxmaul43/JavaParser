# KNPO_Java_Parser

Программа для структурного анализа исходного кода на языке Java. Разбирает входной `.java`-файл на отдельные элементы (классы, перечисления, методы, поля) и сохраняет каждый элемент в отдельный текстовый файл, а также генерирует диаграмму структуры программы в формате DOT (Graphviz).

---

## Содержание

- [Требования](#требования)
- [Сборка](#сборка)
- [Запуск](#запуск)
- [Структура проекта](#структура-проекта)
- [Тесты](#тесты)
  - [Юнит-тесты](#юнит-тесты)
  - [Интеграционные тесты](#интеграционные-тесты)
- [Сборка с покрытием кода](#сборка-с-покрытием-кода)

---

## Требования

| Зависимость | Версия |
|-------------|--------|
| Qt          | 6.x (модуль Core) |
| CMake       | 3.16+ |
| Компилятор  | GCC / Clang / MSVC с поддержкой C++17 |

Поддерживаемые платформы: **Windows**, **Linux**, **macOS**.

---

## Сборка

```bash
# Клонировать репозиторий
git clone <url>
cd KNPO_Java_Parser

# Создать директорию сборки
cmake -S . -B build
cmake --build build
```

После сборки исполняемый файл появится в директории `build/`:
- Linux / macOS: `build/KNPO_Java_Parser`
- Windows: `build/KNPO_Java_Parser.exe`

---

## Запуск

```
./KNPO_Java_Parser <входной_файл.java> <выходная_директория>
```

**Пример:**

```bash
./KNPO_Java_Parser ./Example.java ./output/
```

Программа принимает ровно два аргумента:

| Аргумент | Описание |
|----------|----------|
| `argv[1]` | Путь к входному `.java`-файлу |
| `argv[2]` | Путь к выходной директории для сохранения результатов |

**Коды завершения:**

| Код | Значение |
|-----|----------|
| `0` | Успешное выполнение |
| `1` | Ошибка входных данных (неверное число аргументов, файл не найден и т.п.) |

---

## Структура проекта

```
KNPO_Java_Parser/
├── src/
│   ├── main.cpp
│   ├── parser/
│   │   └── parser.cpp / parser.h
│   ├── io/
│   │   ├── file_reader.cpp / file_reader.h
│   │   ├── file_writer.cpp / file_writer.h
│   │   └── dot_generator.cpp / dot_generator.h
│   └── errors/
│       └── errors.cpp / errors.h
├── tests/
│   ├── CMakeLists.txt
│   ├── unit/
│   │   └── tst_parser.cpp
│   └── integration/
│       ├── run_tests.sh       # запуск интеграционных тестов (Linux / macOS)
│       ├── run_tests.bat      # запуск интеграционных тестов (Windows)
│       └── inputs/            # входные .java-файлы для интеграционных тестов
└── CMakeLists.txt
```

---

## Тесты

### Юнит-тесты

Написаны с использованием Qt Test и собираются вместе с основным проектом.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Для запуска тестового исполняемого файла напрямую:

```bash
./build/tests/tst_parser
```

### Интеграционные тесты

Скрипты находятся в `tests/integration/`. Входные `.java`-файлы для тестов расположены в `tests/integration/inputs/`.

Скрипты прогоняют парсер на всех входных файлах, а также проверяют граничные ситуации (нет аргументов, лишние аргументы, несуществующий файл, пустой файл, файл без прав на чтение). Вывод ошибок для каждого такого теста сохраняется в отдельный лог-файл в директорию `tests/integration/logs/`.

**Linux / macOS:**

```bash
cd tests/integration
bash run_tests.sh
```

**Windows:**

```bat
cd tests\integration
run_tests.bat
```

---

## Сборка с покрытием кода

Для сборки с инструментацией покрытия передайте флаг `-DENABLE_COVERAGE=ON`:

```bash
cmake -S . -B build -DENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build
```

Генерация отчёта через **gcovr**:

```bash
gcovr -r . --html --html-details -o coverage/index.html
```

> Для корректной работы покрытия требуется компилятор с поддержкой `--coverage` (GCC или Clang). На Windows используйте MinGW-w64.

---

## Авторы

- Blinov Lev — June 2026