#include <stdlib.h>

#include "memory.h"
#include "vm.h"

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

static void freeObj(Obj* obj) {
  switch (obj->type) {
    case OBJ_STRING: {
      ObjStr* str = (ObjStr*) obj;
      FREE_ARRAY(char, str->chars, str->len + 1); 
      FREE(ObjStr, obj);
      break;
    }
  }
}

void freeObjects() {
  Obj* obj = vm.objects;
  while (obj != NULL) {
    Obj* next = obj->next;
    freeObj(obj);
    obj = next;
  }
}
