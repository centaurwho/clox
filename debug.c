#include <stdio.h>

#include "debug.h"
#include "value.h"


// Pretty print the chunk. 
void dissassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);

  // Instruction lengths are not all same. Iterate cumulatively
  for (int off = 0; off < chunk->count;) {
    off = dissassembleInstr(chunk, off);
  }
}

// Print OP_CONSTANT
static int constInstr(const char* name, Chunk* chunk, int off) {
  uint8_t constant = chunk->code[off+1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return off + 2;
}

// Print OP_CONSTANT_LONG
static int constLongInstr(const char* name, Chunk* chunk, int off) { 
  uint32_t constant = (chunk->code[off+1] << 16) 
    + (chunk->code[off+2] << 8) + chunk->code[off+3];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return off + 4;
}

// Print simple instructions (OP_RETURN etc.)
static int simpleInstr(const char* name, int off) {
  printf("%s\n", name);
  return off + 1;
}

void dissassLine(Chunk* chunk, int off) {
  int currLine = getLine(chunk, off);
  if (off > 0) {
    int prevLine = getLine(chunk, off - 1);
    if (currLine == prevLine) {
      printf("   | ");
    }
  } else {
    printf("%4d ", currLine);
  }
}

// Print the instruction starting in off
int dissassembleInstr(Chunk* chunk, int off) {
  printf("%04d ", off);

  // Print the line
  dissassLine(chunk, off);

  // Pass to relevant function
  uint8_t instr = chunk->code[off];
  switch (instr) {
    case OP_CONSTANT:
      return constInstr("OP_CONSTANT", chunk, off);
    case OP_CONSTANT_LONG:
      return constLongInstr("OP_CONSTANT_LONG", chunk, off);
    case OP_RETURN:
      return simpleInstr("OP_RETURN", off);
    default:
      printf("Unknown opcode %d\n", instr);
      return off + 1;
  }
}

