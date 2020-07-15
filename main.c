#include <stdio.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
  initVM();
  
  Chunk chunk;
  initChunk(&chunk);  
  int constant = addConstant(&chunk, 1.2);
  
  writeChunk(&chunk, OP_CONSTANT, 4);
  writeChunk(&chunk, constant, 4);
  writeChunk(&chunk, OP_RETURN, 4);
  dissassembleChunk(&chunk, "test_chunk");
  interpret(&chunk);
  freeVM();
  freeChunk(&chunk);  

  return 0;
}
