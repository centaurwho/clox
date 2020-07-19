#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUM,
} ValueType;

// TODO: Improve size of this. (Padding)
typedef struct {
  ValueType type;
  union {
    bool boolean;
    double num;
  } as;
} Value;

#define IS_BOOL(val) ((val).type == VAL_BOOL)
#define IS_NIL(val) ((val).type == VAL_NIL)
#define IS_NUM(val) ((val).type == VAL_NUM)

#define AS_BOOL(val) ((val).as.boolean)
#define AS_NUM(val) ((val).as.num)

#define BOOL_VAL(val) ((Value){ VAL_BOOL, { .boolean = val} })
#define NIL_VAL ((Value){ VAL_NIL, { .num = 0} })
#define NUM_VAL(val) ((Value){ VAL_NUM, { .num = val} })


// TODO: Generify with array struct and functions
// Keeps constants in the program
typedef struct {
  int capacity;
  int count;
  Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value val);
void freeValueArray(ValueArray* array);
void printValue(Value val);

#endif

