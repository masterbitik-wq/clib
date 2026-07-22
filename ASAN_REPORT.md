# Отчёт AddressSanitizer

## Как проверял

В MSYS2 valgrind не работает, поэтому использовал AddressSanitizer.

```bash
make CFLAGS="-fsanitize=address -g -O0" LDFLAGS="-fsanitize=address"
ASAN_OPTIONS=detect_leaks=1 ./test.sh
```

Дополнительно для воспроизведения утечек запускал тесты из `test/leak/`:

```bash
cd test/leak
./run-asan-report.sh reports/before
./run-asan-report.sh reports/after
```

## До исправлений

Файлы: `reports/before/flags-leak.txt`, `reports/before/src-leak.txt`

**Утечка 1 (flags):** при ошибке `asprintf` в цикле по массиву `cflags`/`flags` не освобождалась строка `flag`.

**Утечка 2 (src):** при ошибке добавления файла в список `src` не освобождалась строка `file`.

ASan выдал `LeakSanitizer: detected memory leaks`, exit code 42.

## После исправлений

Файлы: `reports/after/flags-leak.txt`, `reports/after/src-leak.txt`

В `clib_package_new` добавлены:
- `free(flag)` перед `goto cleanup` при ошибке `asprintf`
- `free(file)` в ветках ошибок при обработке `src`

Повторный запуск ASan утечек не показал.

## Коммиты

1. `добавил отчёты asan до исправлений`
2. `исправлена утечка: free(flag) при ошибке asprintf`
3. `исправлена утечка: free(file) в цикле src`
4. `добавил отчёты asan после исправлений`
