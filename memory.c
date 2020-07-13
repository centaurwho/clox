#include <stdlib.h>

#include "memory.h"

void* reallocate(void* prev, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(prev);
    return NULL;
  }
  void* result = realloc(prev, newSize);
  if (result == 0) {
    exit(1);
  }
  return result;
}
