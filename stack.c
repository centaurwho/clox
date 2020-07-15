#include <stdio.h>

#include "stack.h"
#include "memory.h"


void resetStack(Stack* stack) {
  stack->capacity = 0;
  stack->count = 0;
  stack->top = NULL;
  stack->arr = NULL;
}

void push(Stack* stack, Value val) {
  if (stack->capacity < stack->count + 1) {
    int oldCap = stack->capacity;
    stack->capacity = GROW_CAPACITY(oldCap);
    stack->arr = GROW_ARRAY(stack->arr, Value, oldCap, stack->capacity);
  }
  stack->arr[stack->count] = val;
  stack->top = &stack->arr[stack->count];
  stack->count++;
}

Value pop(Stack* stack) {
  if (stack->count > 0) {
    stack->count--;
    return *stack->top--; 
  }
  return *stack->top;
}

Value top(Stack* stack) {
  return *stack->top; 
}

void printStack(const Stack* stack) {
  for (int i = 0; i<stack->count; i++) {
    printf("[ ");
    printValue(stack->arr[i]);
    printf(" ]");
  }
}
