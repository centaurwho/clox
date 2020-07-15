#include <stdio.h>

#include "common.h"
#include "vm.h"
#include "debug.h"

// Global Virtual Machine object. Could have used a VM pointer and
// pass it around.
VM vm;

void initVM() {

}

void freeVM() {

}

// TODO: Direct Threaded Code, Jump Table, Computed Goto.
// For increasing efficiency of bytecode dispatch
static InterpretResult run() {
  #define READ_BYTE() (*vm.ip++)
  // TODO: handle reading long constants?
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  for (;;) {
#ifndef DEBUG_TRACE_EXEC
    dissassembleInstr(vm.chunk, (int)(vm.ip - vm.chunk->code))
#endif
    uint8_t instr;
    switch (instr = READ_BYTE()) {
      case OP_RETURN: 
        return INTERPRET_OK;
      case OP_CONSTANT: { 
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;
      }
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
