# clib

Исправлены две утечки памяти в функции `clib_package_new` (файл `src/common/clib-package.c`).

## Утечки

1. При обработке массива `flags` — добавлен `free(flag)` при ошибке `asprintf`.
2. При обработке массива `src` — добавлен `free(file)` в двух ветках ошибок.

## Проверка

Использовал AddressSanitizer (valgrind в MSYS2 нет):

```bash
make CFLAGS="-fsanitize=address -g -O0" LDFLAGS="-fsanitize=address"
ASAN_OPTIONS=detect_leaks=1 ./test.sh
```

Отчёты:
- `reports/before/` — до исправлений
- `reports/after/` — после исправлений

Подробнее: `ASAN_REPORT.md`
