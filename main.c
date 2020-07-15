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

  writeConstant(&chunk, 12.3, 4);

  writeChunk(&chunk, OP_RETURN, 4);  
  dissassembleChunk(&chunk, "test_chunk");
  freeChunk(&chunk);  
  
  return 0;
}
