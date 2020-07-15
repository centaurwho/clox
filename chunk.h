#ifndef clox_chunk_h
#define clox_chunk_h
 
#include "common.h"
#include "value.h"

// Instruction type
typedef enum {
  OP_RETURN,
  OP_CONSTANT,
} OpCode;

// Main guy. Responsible for all relevant data
typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  
  // Using Run Length Encoding
  int* lines;
  int lastLine;

  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value val);
void freeChunk(Chunk* chunk);
int getLine(Chunk* chunk, int off);

#endif

