#ifndef clox_stack_h
#define clox_stack_h

#include "value.h"

typedef struct {
  Value* arr;
  Value* top;
  int capacity;
  int count;
} Stack;


void resetStack(Stack* stack);
void push(Stack* stack, Value val);
Value pop(Stack* stack);
Value top(Stack* stack);
void printStack(const Stack* stack);

#endif

