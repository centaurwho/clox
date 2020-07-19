#ifndef clox_memory_h
#define clox_memory_h
 
#include "common.h"

#define ALLOCATE(type, count) \
  (type*) reallocate(NULL, 0, sizeof(type) * count);

// Start the capacity from 8 and when it is no longer enough, just 
// double it.
#define GROW_CAPACITY(capacity) \
  ((capacity) < 8 ? 8 : (capacity) * 2)


// Prepare the args and pass to reallocate
#define GROW_ARRAY(prev, type, oldCount, count) \
  (type*)reallocate(prev, sizeof(type) * (oldCount), \
      sizeof(type) * (count))

// Prepare the args and pass to reallocate
#define FREE_ARRAY(type, pointer, oldCount) \
  reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* prev, size_t oldSize, size_t newSize);

#endif

