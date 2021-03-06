#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"

// Global Virtual Machine object. Could have used a VM pointer and
// pass it around.
VM vm;

void initVM() {
  resetStack(&vm.stack);
  vm.objects = NULL;
  initTable(&vm.strings);
  initTable(&vm.globals);
}

void freeVM() {
  freeTable(&vm.globals);
  freeTable(&vm.strings);
  freeObjects();
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

static void pushVal(Value val) {
  push(&vm.stack, val);
}

static Value peek(int dist) {
  return vm.stack.top[-dist];
}

static bool isFalsey(Value val) {
  return IS_NIL(val) || (IS_BOOL(val) && !AS_BOOL(val));
}

static void concat() {
  ObjStr* b = AS_STRING(popVal());
  ObjStr* a = AS_STRING(popVal());

  int len = a->len + b->len;
  char* chars = ALLOCATE(char, len + 1);
  memcpy(chars, a->chars, a->len);
  memcpy(chars + a->len, b->chars, b->len);
  chars[len] = '\0';

  ObjStr* res = takeStr(chars, len);
  pushVal(OBJ_VAL(res));
}



// TODO: Direct Threaded Code, Jump Table, Computed Goto.
// For increasing efficiency of bytecode dispatch
static InterpretResult run() {
  #define READ_BYTE() (*vm.ip++)
  // TODO: handle reading long constants?
  #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
  #define READ_STR() AS_STRING(READ_CONSTANT())

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
      // TODO: Seperate string concat and addition
      case OP_ADD:
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concat();
        } else if (IS_NUM(peek(0)) && IS_NUM(peek(1))) {
          double b = AS_NUM(popVal());
          double a = AS_NUM(popVal());
          pushVal(NUM_VAL(a + b));
        } else {
          runtimeErr("Operands must be two strings or two nums"); 
          return INTERPRET_RUNTIME_ERROR;
        }
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
      case OP_NOT:
        pushVal(BOOL_VAL(isFalsey(popVal())));
        break;
      case OP_POP:
        popVal();
        break;
      case OP_PRINT:
        printValue(popVal());
        printf("\n");
        break;
      case OP_RETURN: 
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
      case OP_EQUAL: {
        Value b = popVal();
        Value a = popVal();
        pushVal(BOOL_VAL(valuesEq(a, b)));
        break;
      }
      case OP_GREATER:
        BINARY_OP(BOOL_VAL, >);
        break;
      case OP_LESS:
        BINARY_OP(BOOL_VAL, <);
        break;
      case OP_DEF_GLOBAL: {
        ObjStr* name = READ_STR();
        addEntry(&vm.globals, name, peek(0)); 
        popVal();
        break;
      }
      case OP_GET_GLOBAL: {
        ObjStr* name = READ_STR();
        Value val;
        if (!getEntry(&vm.globals, name, &val)) {
          runtimeErr("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        pushVal(val);
        break;
      }
      case OP_SET_GLOBAL: {
        ObjStr* name = READ_STR();
        if (addEntry(&vm.globals, name, peek(0))) {
          delEntry(&vm.globals, name);
          runtimeErr("Undefined variable '%s',", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }        
        break;
      }
    }
  }
  #undef READ_BYTE 
  #undef READ_CONSTANT 
  #undef READ_STR
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
