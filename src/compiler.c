#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

typedef struct {
  Token curr;
  Token prev;
  bool hadErr;
  // TODO: may use setjmp and longjmp instead of this flag
  bool panicMode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGN, // = 
  PREC_OR, // or
  PREC_AND, // and
  PREC_EQ, // == != 
  PREC_COMP, // < > <= >=
  PREC_TERM, // + -
  PREC_FACTOR, // * /
  PREC_UNARY, // -
  PREC_CALL, // . ()
  PREC_PRIMARY,
} Precedence;

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence prec;
} ParseRule;

Parser parser;

Chunk* compilingChunk;

static Chunk* currChunk() {
  return compilingChunk;  
}

static void errorAt(Token* tok, const char* msg) {
  if (parser.panicMode) {
    return;
  }
  parser.panicMode = true;
  fprintf(stderr, "Error on line: %d.",tok-> line);

  if (tok->type == TOKEN_EOF) {
    fprintf(stderr, " End of file");
  } else if (tok->type == TOKEN_ERROR) {
  
  } else {
    fprintf(stderr, " %s\n", msg);
  }
}

static void error(const char* msg) {
  errorAt(&parser.prev, msg);
}

static void errAtCurr(const char* msg) {
  errorAt(&parser.curr, msg);
}

static void advance() {
  parser.prev = parser.curr;

  for (;;) {
    parser.curr = scanToken();
    // printToken(&parser.curr);
    if (parser.curr.type != TOKEN_ERROR) {
      break;
    }

    errAtCurr(parser.curr.start);
  }
}

static void consume(TokenType type, const char* msg) {
  if (parser.curr.type == type) {
    advance();
    return;
  }
  errAtCurr(msg);
}

static void emitByte(uint8_t byte) {
  writeChunk(currChunk(), byte, parser.prev.line);
}

static void emitBytes(uint8_t b1, uint8_t b2) {
  emitByte(b1);
  emitByte(b2);
}

static void emitReturn() {
  emitByte(OP_RETURN);
}

// TODO: Handle long constants
static uint8_t makeConstant(Value val) {
  int constant = addConstant(currChunk(), val);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk");
    return 0;
  }

  return (uint8_t) constant;
}

static void emitConstant(Value val) {
  emitBytes(OP_CONSTANT, makeConstant(val));
}

static void endCompiler() {
  emitReturn();
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadErr) {
    dissassembleChunk(currChunk(), "code");
  }
#endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrec(Precedence prec);

static void binary() {
  // store the operator
  TokenType opType = parser.prev.type;

  // Compile right operand
  ParseRule* rule = getRule(opType);
  parsePrec((Precedence)(rule->prec + 1));

  // Emit the operator
  switch (opType) {
    case TOKEN_PLUS:
      emitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      emitByte(OP_SUBTRACT);
      break;
    case TOKEN_STAR:
      emitByte(OP_MULTIPLY);
      break;
    case TOKEN_SLASH:
      emitByte(OP_DIVIDE);
      break;
    default:
      return;
  }
}

static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expected ')'");
}

static void number() {
  double val = strtod(parser.prev.start, NULL);
  emitConstant(val);
}

static void unary() {
  TokenType opType = parser.prev.type;

  // Compile the operand
  parsePrec(PREC_UNARY);

  // Emit op instr
  switch (opType) {
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;
  }
}

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
  [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
  [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
  [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
  [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
  [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
  [TOKEN_MINUS] = {unary, binary, PREC_TERM},
  [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
  [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
  [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
  [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
  [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
  [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
  [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_ID] = {NULL, NULL, PREC_NONE},
  [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
  [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
  [TOKEN_AND] = {NULL, NULL, PREC_NONE},
  [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
  [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
  [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
  [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
  [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
  [TOKEN_IF] = {NULL, NULL, PREC_NONE},
  [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
  [TOKEN_OR] = {NULL, NULL, PREC_NONE},
  [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
  [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
  [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
  [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
  [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
  [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
  [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
  [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
  [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};


static void parsePrec(Precedence prec) {
  advance();
  ParseFn prefixRule = getRule(parser.prev.type)->prefix;
  if (prefixRule == NULL ) {
    error("Expect expression");
    return;
  } 

  prefixRule();

  while (prec <= getRule(parser.curr.type)->prec) {
    advance();
    ParseFn infixRule = getRule(parser.prev.type)->infix;
    infixRule();
  }
}

static ParseRule* getRule(TokenType type) {
  return &rules[type];
} 

static void expression() {
  parsePrec(PREC_ASSIGN);
}

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

  parser.hadErr = false;
  parser.panicMode = false;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  return !parser.hadErr;
}
