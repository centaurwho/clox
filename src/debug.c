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
    case OP_NIL:
      return simpleInstr("OP_NIL", off);
    case OP_TRUE:
      return simpleInstr("OP_TRUE", off);
    case OP_FALSE:
      return simpleInstr("OP_FALSE", off);
    case OP_ADD:
      return simpleInstr("OP_ADD", off);
    case OP_SUBTRACT:
      return simpleInstr("OP_SUBTRACT", off);
    case OP_MULTIPLY:
      return simpleInstr("OP_MULTIPLY", off);
    case OP_DIVIDE:
      return simpleInstr("OP_DIVIDE", off);
    case OP_NOT:
      return simpleInstr("OP_NOT", off);
    case OP_NEGATE:
      return simpleInstr("OP_NEGATE", off);
    case OP_RETURN:
      return simpleInstr("OP_RETURN", off);
    default:
      printf("Unknown opcode %d\n", instr);
      return off + 1;
  }
}

// TODO: Use some ugly macro magic instead of data dupping
const char* tokNames[] = { 
  [TOKEN_LEFT_PAREN] = "TOKEN_LEFT_PAREN", 
  [TOKEN_RIGHT_PAREN] = "TOKEN_RIGHT_PAREN",
  [TOKEN_LEFT_BRACE] = "TOKEN_LEFT_BRACE",
  [TOKEN_RIGHT_BRACE] = "TOKEN_RIGHT_BRACE",
  [TOKEN_COMMA] = "TOKEN_COMMA",
  [TOKEN_DOT] = "TOKEN_DOT", 
  [TOKEN_MINUS] = "TOKEN_MINUS",
  [TOKEN_PLUS] = "TOKEN_PLUS",
  [TOKEN_SEMICOLON] = "TOKEN_SEMICOLON",
  [TOKEN_SLASH] = "TOKEN_SLASH",
  [TOKEN_STAR] = "TOKEN_STAR",
  [TOKEN_BANG] = "TOKEN_BANG",
  [TOKEN_EQUAL] = "TOKEN_EQUAL",
  [TOKEN_EQUAL_EQUAL] = "TOKEN_EQUAL_EQUAL",
  [TOKEN_GREATER] = "TOKEN_GREATER", 
  [TOKEN_GREATER_EQUAL] = "TOKEN_GREATER_EQUAL",
  [TOKEN_LESS] = "TOKEN_LESS",
  [TOKEN_LESS_EQUAL] = "TOKEN_LESS_EQUAL",
  [TOKEN_ID] = "TOKEN_ID", 
  [TOKEN_STRING] = "TOKEN_STRING",
  [TOKEN_NUMBER] = "TOKEN_NUMBER",
  [TOKEN_AND] = "TOKEN_AND", 
  [TOKEN_OR] = "TOKEN_OR",
  [TOKEN_NOT] = "TOKEN_NOT", 
  [TOKEN_IF] = "TOKEN_IF",
  [TOKEN_ELSE] = "TOKEN_ELSE", 
  [TOKEN_FOR] = "TOKEN_FOR", 
  [TOKEN_WHILE] = "TOKEN_WHILE",
  [TOKEN_FALSE] = "TOKEN_FALSE",
  [TOKEN_TRUE] = "TOKEN_TRUE",
  [TOKEN_CLASS] = "TOKEN_CLASS",
  [TOKEN_SUPER] = "TOKEN_SUPER", 
  [TOKEN_THIS] = "TOKEN_THIS", 
};

void printToken(Token* tok) {
  const char* type = tokNames[tok->type];
  printf("%s %s %d %d\n", type, tok->start, tok->len, tok->line);
}


