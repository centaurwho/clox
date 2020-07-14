#include <stdlib.h>
#include <stdio.h>

#include "value.h"
#include "memory.h"

void initValueArray(ValueArray* array) {
  array->count = 0;
  array->capacity = 0;
  array->values = NULL;
}

void writeValueArray(ValueArray* array, uint8_t byte) {
  if (array->capacity < array->count + 1) {
    int oldCap = array->capacity;
    array->capacity = GROW_CAPACITY(oldCap);
    array->values = GROW_ARRAY(array->values, Value, oldCap, 
        array->capacity); 
  }

  array->values[array->count] = byte;
  array->count++;
}

void freeValueArray(ValueArray* array) {
  FREE_ARRAY(uint8_t, array->values, array->capacity);
  initValueArray(array);
}

void printValue(Value val) {
  printf("%g", val);
}
