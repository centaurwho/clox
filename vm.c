#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

// Global Virtual Machine object. Could have used a VM pointer and
// pass it around.
VM vm;

void initVM() {
  resetStack(&vm.stack);
}

void freeVM() {

}

static Value popVal() {
  return pop(&vm.stack);
}

static void pushVal(Value val) {
  push(&vm.stack, val);
}

// TODO: Direct Threaded Code, Jump Table, Computed Goto.
// For increasing efficiency of bytecode dispatch
static InterpretResult run() {
  #define READ_BYTE() (*vm.ip++)
  // TODO: handle reading long constants?
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  #define BINARY_OP(op) \
    do { \
      double b = popVal(); \
      double a = popVal(); \
      pushVal(a op b); \
    } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXEC
    printf("    ");
    printStack(&vm.stack);
    printf("\n");
    dissassembleInstr(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instr;
    switch (instr = READ_BYTE()) {
      case OP_NEGATE: 
        pushVal(-popVal()); 
        break;
      case OP_ADD:
        BINARY_OP(+); 
        break;
      case OP_SUBTRACT:
        BINARY_OP(-); 
        break;
      case OP_MULTIPLY:
        BINARY_OP(*); 
        break;
      case OP_DIVIDE:
        BINARY_OP(/); 
        break;
      case OP_RETURN: 
        printValue(popVal());
        printf("\n");
        return INTERPRET_OK;
      case OP_CONSTANT:  
        pushVal(READ_CONSTANT());
        break;
      
    }
  }
  #undef READ_BYTE 
  #undef READ_CONSTANT 
  #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  compile(source);
  return INTERPRET_OK;
}
