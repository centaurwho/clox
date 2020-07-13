#include <stdio.h>

#include "debug.h"




void dissassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);

  for (int off = 0; off < chunk->count;) {
    off = dissassembleInstr(chunk, off);
  }
}

static int simpleInstr(const char* name, int off) {
  printf("%s\n", name);
  return off + 1;
}

int dissassembleInstr(Chunk* chunk, int off) {
  printf("%04d ", off);

  uint8_t instr = chunk->code[off];
  switch (instr) {
    case OP_RETURN:
      return simpleInstr("OP_RETURN", off);
    default:
      printf("Unknown opcode %d\n", instr);
      return off + 1;
  }
}
