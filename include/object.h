#ifndef clox_object_h
#define clox_object_h
 
#include "common.h"
#include "value.h"

#define OBJ_TYPE(val) (AS_OBJ(val)->type)
#define IS_STRING(val) isObjType(val, OBJ_STRING)
#define AS_STRING(val) ((ObjStr*)AS_OBJ(val))
#define AS_CSTRING(val) (((ObjStr*)AS_OBJ(val))->chars)

typedef enum {
  OBJ_STRING,
} ObjType;

struct sObj {
  ObjType type;
  struct sObj* next;
};

struct sObjStr {
  Obj obj;
  int len;
  char* chars;
  uint32_t hash;
};

ObjStr* takeStr(char* chars, int len);
ObjStr* copyStr(const char* chars, int len);
void printObj(Value val);

static inline bool isObjType(Value val, ObjType type) {
  return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

#endif

