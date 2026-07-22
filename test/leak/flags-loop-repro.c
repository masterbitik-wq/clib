#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int vasprintf_local(char **str, const char *fmt, va_list args) {
  int size = 0;
  va_list tmpa;

  va_copy(tmpa, args);
  size = vsnprintf(NULL, 0, fmt, tmpa);
  va_end(tmpa);

  if (size < 0) {
    return -1;
  }

  *str = malloc(size + 1);
  if (NULL == *str) {
    return -1;
  }

  size = vsprintf(*str, fmt, args);
  return size;
}

static int fail_asprintf = 0;
static int asprintf_calls = 0;

static int pkg_asprintf(char **strp, const char *fmt, ...) {
  va_list ap;
  int rc;

  if (fail_asprintf && strstr(fmt, "%s %s")) {
    asprintf_calls++;
    if (asprintf_calls >= 2) {
      return -1;
    }
  }

  va_start(ap, fmt);
  rc = vasprintf_local(strp, fmt, ap);
  va_end(ap);
  return rc;
}

static char *strdup_local(const char *s) {
  size_t len = strlen(s) + 1;
  char *copy = malloc(len);
  if (copy) {
    memcpy(copy, s, len);
  }
  return copy;
}

int main(void) {
  const char *flags[] = {"-Wall", "-O2"};
  char *pkg_flags = NULL;
  int error = 1;

  fail_asprintf = 1;

  for (unsigned int i = 0; i < 2; i++) {
    char *flag = strdup_local(flags[i]);
    if (flag) {
      if (!pkg_flags) {
        pkg_flags = "";
      }
      if (-1 == pkg_asprintf(&pkg_flags, "%s %s", pkg_flags, flag)) {
        free(flag);
        goto cleanup;
      }
      free(flag);
    }
  }

  error = 0;

cleanup:
  if (pkg_flags && pkg_flags[0] != '\0') {
    free(pkg_flags);
  }

  return error;
}
