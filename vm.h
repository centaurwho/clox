#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
  Chunk* chunk;
  // TODO: Possible optimiation. Use as a local variable somewhere
  uint8_t* ip; // Keep instruction pointer here.
  // TODO: Why not use dynamic array
  Value stack[STACK_MAX];
  Value* top;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);
void push(Value val);
Value pop();

#endif

