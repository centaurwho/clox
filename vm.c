#include <stdio.h>

#include "common.h"
#include "vm.h"
#include "debug.h"

// Global Virtual Machine object. Could have used a VM pointer and
// pass it around.
VM vm;

static void resetStack() {
  vm.top = vm.stack;
}

void initVM() {
  resetStack();
}

void freeVM() {

}

// TODO: edge cases in push and pop
void push(Value val) {
  *vm.top = val;
  vm.top++;
}

Value pop() {
  vm.top--;
  return *vm.top;
}

// TODO: Direct Threaded Code, Jump Table, Computed Goto.
// For increasing efficiency of bytecode dispatch
static InterpretResult run() {
  #define READ_BYTE() (*vm.ip++)
  // TODO: handle reading long constants?
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  for (;;) {
#ifdef DEBUG_TRACE_EXEC
    printf("    ");
    for (Value* slot = vm.stack; slot < vm.top; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    dissassembleInstr(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instr;
    switch (instr = READ_BYTE()) {
      case OP_NEGATE: 
        push(-pop()); 
        break;
      case OP_RETURN: 
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      case OP_CONSTANT:  
        push(READ_CONSTANT());
        break;
      
    }
  }
  #undef READ_BYTE 
  #undef READ_CONSTANT 
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}
