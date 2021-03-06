#ifndef clox_chunk_h
#define clox_chunk_h
 
#include "common.h"
#include "value.h"

// Instruction type
typedef enum {
  OP_POP,
  OP_PRINT,
  OP_RETURN,
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_DEF_GLOBAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
} OpCode;

// Main guy. Responsible for all relevant data
typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  
  // Using Run Length Encoding
  // TODO: Using just and int array is efficient unless there are multiple
  // empty lines. Then I need a new logic. 
  int* lines;
  int lastLine;

  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value val);
void writeConstant(Chunk* chunk, Value val, int line);
void freeChunk(Chunk* chunk);
int getLine(Chunk* chunk, int off);

#endif

