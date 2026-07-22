# Отчёт AddressSanitizer: утечки памяти в `clib_package_new`

## Методика

В среде без Valgrind (MSYS2/Linux) для обнаружения утечек использован **AddressSanitizer (ASan)**.

Сборка и запуск тестов:

```bash
cd test/leak
./run-asan-report.sh reports/before   # до исправлений
./run-asan-report.sh reports/after    # после всех исправлений
```

Флаги сборки:

```bash
make CC=gcc CFLAGS="-std=c99 -Wall -I../../src/common -I../../deps -DHAVE_PTHREADS -pthread -g -fsanitize=address -O0 -fno-omit-frame-pointer" LDFLAGS="-lcurl -fsanitize=address"
ASAN_OPTIONS=detect_leaks=1:exitcode=42
```

## Тестовые сценарии

| Тест | Файл | Что проверяется |
|------|------|-----------------|
| Утечка №1 (flags) | `test/leak/flags-loop-repro.c` | Ошибка `asprintf` при конкатенации массива `cflags`/`flags` — тот же код, что в `clib_package_new` |
| Утечка №2 (src) | `test/leak/package-leak-src.c` | Ошибка `list_node_new` при добавлении элемента в `src` — вызов реальной `clib_package_new` |

Для воспроизведения ошибочных веток используются контролируемые сбои:
- сценарий flags: принудительный возврат `-1` из `asprintf` на второй итерации;
- сценарий src: `FAIL_LIST_NODE_NEW=1` (перехват `list_node_new` через `--wrap`).

---

## ДО исправлений

### Утечка №1 — обработка массива `flags` / `cflags`

**Файл:** `src/common/clib-package.c`, цикл по массиву flags  
**Проблема:** при ошибке `asprintf` строка `flag` не освобождалась перед `goto cleanup`.

**Вывод ASan** (`reports/before/flags-leak.txt`):

```
==7533==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 7 byte(s) in 1 object(s) allocated from:
    #0 ... in malloc ...
    #4 ... in main .../flags-loop-repro.c:70

SUMMARY: AddressSanitizer: 7 byte(s) leaked in 1 allocation(s).
Exit code: 42
```

### Утечка №2 — обработка массива `src` / `files`

**Файл:** `src/common/clib-package.c`, цикл по массиву src  
**Проблема:** при ошибке добавления в список (`list_rpush` / `list_node_new`) строка `file` не освобождалась.

**Вывод ASan** (`reports/before/src-leak.txt`):

```
==7535==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 6 byte(s) in 1 object(s) allocated from:
    #0 ... in malloc ...
    #1 ... in strdup .../strdup.c:24
    #2 ... in json_array_get_string_safe .../clib-package.c:198
    #3 ... in clib_package_new .../clib-package.c:550

SUMMARY: AddressSanitizer: 6 byte(s) leaked in 1 allocation(s).
Exit code: 42
```

---

## ПОСЛЕ исправлений

### Коммит 1: исправление утечки в flags

Добавлен `free(flag)` перед `goto cleanup` при ошибке `asprintf`.

**Результат** (`reports/after-fix1/flags-leak.txt`): утечка не обнаружена (exit code: 1).  
**Результат** (`reports/after-fix1/src-leak.txt`): утечка src **ещё присутствует** (exit code: 42).

### Коммит 2: исправление утечки в src

Добавлен `free(file)` в ветках ошибок при `!file` и при неудачном `list_rpush`.

**Результат** (`reports/after/flags-leak.txt`): утечка не обнаружена (exit code: 1).  
**Результат** (`reports/after/src-leak.txt`): утечка не обнаружена (exit code: 0).

---

## Внесённые изменения в `clib_package_new`

```c
// Утечка №1 — flags
if (-1 == asprintf(&pkg->flags, "%s %s", pkg->flags, flag)) {
    free(flag);   // добавлено
    goto cleanup;
}

// Утечка №2 — src
if (!file) {
    free(file);   // добавлено
    goto cleanup;
}
if (!(list_rpush(pkg->src, list_node_new(file)))) {
    free(file);   // добавлено
    goto cleanup;
}
```

## История коммитов

1. `Add ASan leak tests and before-fix memory reports` — тесты и отчёт «до»
2. `Fix memory leak in flags array handling in clib_package_new` — исправление утечки №1
3. `Fix memory leak in src array handling in clib_package_new` — исправление утечки №2
