#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "stack.h"

typedef struct {
  Chunk* chunk;
  uint8_t* ip; // Keep instruction pointer here.
  Stack stack;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);

#endif

