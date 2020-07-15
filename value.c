#include <stdlib.h>
#include <stdio.h>

#include "value.h"
#include "memory.h"


// Reset the value array
void initValueArray(ValueArray* array) {
  array->count = 0;
  array->capacity = 0;
  array->values = NULL;
}

// Write the constant to the value array
void writeValueArray(ValueArray* array, uint8_t byte) {
  if (array->capacity < array->count + 1) {
    // capacity is not enough. Grow
    int oldCap = array->capacity;
    array->capacity = GROW_CAPACITY(oldCap);
    array->values = GROW_ARRAY(array->values, Value, oldCap, 
        array->capacity); 
  }

  // Do the actual writing.
  array->values[array->count] = byte;
  array->count++;
}

// Free the value array and reset 
void freeValueArray(ValueArray* array) {
  FREE_ARRAY(uint8_t, array->values, array->capacity);
  initValueArray(array);
}

// For now values are all doubles. Just print
void printValue(Value val) {
  printf("%g", val);
}
