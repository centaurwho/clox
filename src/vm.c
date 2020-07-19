#include <stdarg.h>
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

static void runtimeErr(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instr = vm.ip - vm.chunk->code - 1;
  int line = getLine(vm.chunk, -instr);
  fprintf(stderr, "[line %d] in script\n", line);
}

static Value popVal() {
  return pop(&vm.stack);
}

static Value peek(int dist) {
  return vm.stack.top[-dist];
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

  #define BINARY_OP(type, op) \
    do { \
      if (!IS_NUM(peek(0)) || !(IS_NUM(peek(1)))) { \
        runtimeErr("Operands must be numbers"); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUM(popVal()); \
      double a = AS_NUM(popVal()); \
      pushVal(type(a op b)); \
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
        if (!IS_NUM(peek(0))) {
          runtimeErr("Operand must be a number");
          return INTERPRET_RUNTIME_ERROR;
        }  
        pushVal(NUM_VAL(-AS_NUM(popVal()))); 
        break;
      case OP_ADD:
        BINARY_OP(NUM_VAL, +); 
        break;
      case OP_SUBTRACT:
        BINARY_OP(NUM_VAL, -); 
        break;
      case OP_MULTIPLY:
        BINARY_OP(NUM_VAL, *); 
        break;
      case OP_DIVIDE:
        BINARY_OP(NUM_VAL, /); 
        break;
      case OP_RETURN: 
        printValue(popVal());
        printf("\n");
        return INTERPRET_OK;
      case OP_CONSTANT:  
        pushVal(READ_CONSTANT());
        break;
      case OP_NIL:
        pushVal(NIL_VAL);
        break;
      case OP_TRUE:
        pushVal(BOOL_VAL(true));
        break;
      case OP_FALSE:
        pushVal(BOOL_VAL(false));
        break;
    }
  }
  #undef READ_BYTE 
  #undef READ_CONSTANT 
  #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  Chunk chunk;
  initChunk(&chunk);
  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }
  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult res = run();
  
  freeChunk(&chunk);
  return res;
}
