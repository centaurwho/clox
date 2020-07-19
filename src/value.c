#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "value.h"
#include "memory.h"

bool valuesEq(Value a, Value b) {
  if (a.type != b.type) {
    return false;
  }
  switch (a.type) {
    case VAL_BOOL:
      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
      return true;
    case VAL_NUM:
      return AS_NUM(a) == AS_NUM(b);
    case VAL_OBJ: {
      ObjStr* stra = AS_STRING(a);
      ObjStr* strb = AS_STRING(b);
      return stra->len == strb->len && memcmp(stra->chars, strb->chars,
          stra->len) == 0;
    }
  }
  return false; // Can not reach here but c warns me somehow
}

// Reset the value array
void initValueArray(ValueArray* array) {
  array->count = 0;
  array->capacity = 0;
  array->values = NULL;
}

// Write the constant to the value array
void writeValueArray(ValueArray* array, Value val) {
  if (array->capacity < array->count + 1) {
    // capacity is not enough. Grow
    int oldCap = array->capacity;
    array->capacity = GROW_CAPACITY(oldCap);
    array->values = GROW_ARRAY(array->values, Value, oldCap, 
        array->capacity); 
  }

  // Do the actual writing.
  array->values[array->count] = val;
  array->count++;
}

// Free the value array and reset 
void freeValueArray(ValueArray* array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}

// For now values are all doubles. Just print
void printValue(Value val) {
  switch (val.type) {
    case VAL_BOOL:
      printf(AS_BOOL(val) ? "true" : "false");
      break;
    case VAL_NIL:
      printf("nil");
      break;
    case VAL_NUM:
      printf("%g", AS_NUM(val));
      break;
    case VAL_OBJ:
      printObj(val);
      break;
  }
}
