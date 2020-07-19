#ifndef clox_debug_h
#define clox_debug_h
 
#include "chunk.h"
#include "scanner.h"

void dissassembleChunk(Chunk* chunk, const char* name);
int dissassembleInstr(Chunk* chunk, int off);
void printToken(Token* tok);

#endif

