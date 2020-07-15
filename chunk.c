#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

#include <stdio.h>
// Reset the chunk for use later
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  chunk->lastLine = 0;
  initValueArray(&chunk->constants);
}

// Write the byte and line number to the chunk
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    // capacity is not enough. Grow
    int oldCap = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCap);
    chunk->code = GROW_ARRAY(chunk->code, uint8_t, oldCap, 
        chunk->capacity); 
  }
  if (chunk->lastLine < line) {    
    chunk->lines = GROW_ARRAY(chunk->lines, int, chunk->lastLine, line);
    for (int i = chunk->lastLine; i <= line; i++) {
      chunk->lines[i] = 0;
    }
  }
  // Do the actual writing
  chunk->code[chunk->count] = byte;
  chunk->lines[line-1]++;
  chunk->lastLine = line;
  chunk->count++;
}

int addConstant(Chunk* chunk, Value val) {
  writeValueArray(&chunk->constants, val);
  return chunk->constants.count - 1;
}

// Adds a long constant to the chunk
void writeConstant(Chunk* chunk, Value val, int line) {
  writeChunk(chunk, OP_CONSTANT_LONG, line);
  int cons = addConstant(chunk, val);
  writeChunk(chunk, cons >> 16, line);
  writeChunk(chunk, (cons >> 8) | 0xff00, line);
  writeChunk(chunk, (uint8_t) cons, line);
}

// Free the chunk and its contents and reset
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->lastLine);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

// Decode the RLE
int getLine(Chunk* chunk, int off) {
  const int* lines = chunk->lines;
  int i = 0;
  for (int sum = 0; sum <= off && i < chunk->lastLine; i++) {
    sum += lines[i];
  }
  return i;
}
