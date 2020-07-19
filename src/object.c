#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objType) \
  (type*) allocateObj(sizeof(type), objType)

static Obj* allocateObj(size_t size, ObjType type) {
  Obj* obj = (Obj*)reallocate(NULL, 0, size);
  obj->type = type;
  return obj;
}

static ObjStr* allocateStr(char* chars, int len) {
  ObjStr* str = ALLOCATE_OBJ(ObjStr, OBJ_STRING);
  str->len = len;
  str->chars = chars;

  return str;
}

ObjStr* takeStr(char* chars, int len) {
  return allocateStr(chars, len);
}

ObjStr* copyStr(const char* chars, int len) {
  char* heapChars = ALLOCATE(char, len + 1);
  memcpy(heapChars, chars, len);
  heapChars[len] = '\0';

  return allocateStr(heapChars, len);
}

void printObj(Value val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(val));
      break;
  }
}
