#include "common.h"
#include "chunk.h"
#include "debug.h"

#include <stdio.h>

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);  
 
  // int l[] = {1,2,3,4};
  // chunk.lines = l; 
  // chunk.lastLine = 4;

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 12);
  writeChunk(&chunk, constant, 12);

  writeChunk(&chunk, OP_RETURN, 12);  
  dissassembleChunk(&chunk, "test_chunk");
  freeChunk(&chunk);  
  
  return 0;
}
