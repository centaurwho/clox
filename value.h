#ifndef clox_value_h
#define clox_value_h

#include "common.h"

// TODO: Generify with array struct and functions
typedef double Value;

typedef struct {
  int capacity;
  int count;
  Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, uint8_t byte);
void freeValueArray(ValueArray* array);
void printValue(Value val);

#endif

