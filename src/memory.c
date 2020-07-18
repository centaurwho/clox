#include <stdlib.h>

#include "memory.h"

// Wraps around realloc and free. All memory actions take place in here
// so when we implement GC it can be gracefully handled here
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
