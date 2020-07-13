#ifndef clox_debug_h
#define clox_debug_h
 
#include "chunk.h"

void dissassembleChunk(Chunk* chunk, const char* name);
int dissassembleInstr(Chunk* chunk, int offset);

#endif

