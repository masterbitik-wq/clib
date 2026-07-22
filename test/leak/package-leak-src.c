#include <stdio.h>
#include <stdlib.h>

#include "clib-package.h"

int main(void) {
  const char *json =
      "{"
      "  \"name\": \"leak-test\","
      "  \"version\": \"1.0.0\","
      "  \"repo\": \"author/leak-test\","
      "  \"src\": [\"foo.c\", \"bar.c\"]"
      "}";

  clib_package_t *pkg = clib_package_new(json, 0);
  if (pkg) {
    clib_package_free(pkg);
  }

  return 0;
}
