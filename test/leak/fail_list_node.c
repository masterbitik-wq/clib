#include <stdlib.h>

#include "list/list.h"

list_node_t *__real_list_node_new(void *val);

list_node_t *__wrap_list_node_new(void *val) {
  if (getenv("FAIL_LIST_NODE_NEW")) {
    return NULL;
  }

  return __real_list_node_new(val);
}
