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

  // Add object to virtual machines linked list.
  obj->next = vm.objects;
  vm.objects = obj;
  return obj;
}

static ObjStr* allocateStr(char* chars, int len, uint32_t hash) {
  ObjStr* str = ALLOCATE_OBJ(ObjStr, OBJ_STRING);
  str->len = len;
  str->chars = chars;
  str->hash = hash;
  return str;
}

// See FNV-1a. Short and good enough
static uint32_t hashStr(const char* key, int len) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < len; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjStr* takeStr(char* chars, int len) {
  uint32_t hash = hashStr(chars, len);
  return allocateStr(chars, len, hash);
}

ObjStr* copyStr(const char* chars, int len) {
  uint32_t hash = hashStr(chars, len);
  char* heapChars = ALLOCATE(char, len + 1);
  memcpy(heapChars, chars, len);
  heapChars[len] = '\0';

  return allocateStr(heapChars, len, hash);
}

void printObj(Value val) {
  switch (OBJ_TYPE(val)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(val));
      break;
  }
}
