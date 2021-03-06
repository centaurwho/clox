#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "object.h"
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

typedef void (*ParseFn)(bool canAssign);

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

static bool check(TokenType type) {
  return parser.curr.type == type;
}

static bool match(TokenType type) {
  if (!check(type)) {
    return false;
  }
  advance();
  return true;
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
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrec(Precedence prec);

static uint8_t idConstant(Token* tok) {
  return makeConstant(OBJ_VAL(copyStr(tok->start, tok->len)));
}

static uint8_t parseVar(const char* errMsg) {
  consume(TOKEN_ID, errMsg);
  return idConstant(&parser.prev);
}

static void defineVar(uint8_t global) {
  emitBytes(OP_DEF_GLOBAL, global);
}

static void binary(bool canAssign) {
  // store the operator
  TokenType opType = parser.prev.type;

  // Compile right operand
  ParseRule* rule = getRule(opType);
  parsePrec((Precedence)(rule->prec + 1));

  // Emit the operator
  switch (opType) {
    case TOKEN_EQUAL_EQUAL:
      emitByte(OP_EQUAL);
      break;
    case TOKEN_BANG_EQUAL:
      emitBytes(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_GREATER:
      emitByte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      emitBytes(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      emitByte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      emitBytes(OP_GREATER, OP_NOT);
      break;
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

static void literal(bool canAssign) {
  switch (parser.prev.type) {
    case TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    case TOKEN_NIL:
      emitByte(OP_NIL);
      break;
    default:
      break;
  }
}

static void grouping(bool canAssign) {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expected ')'");
}

static void number(bool canAssign) {
  double val = strtod(parser.prev.start, NULL);
  emitConstant(NUM_VAL(val));
}

// TODO: Escape characters should be treated around here
static void string(bool canAssign) {
  emitConstant(OBJ_VAL(copyStr(parser.prev.start+1, 
          parser.prev.len - 2)));
}

static void namedVar(Token name, bool canAssign) {
  uint8_t arg = idConstant(&name);
  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(OP_SET_GLOBAL, arg);
  } else {
    emitBytes(OP_GET_GLOBAL, arg);
  }
}

static void variable(bool canAssign) {
  namedVar(parser.prev, canAssign);
}

static void unary(bool canAssign) {
  TokenType opType = parser.prev.type;

  // Compile the operand
  parsePrec(PREC_UNARY);

  // Emit op instr
  switch (opType) {
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    case TOKEN_BANG:
      emitByte(OP_NOT);
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
  [TOKEN_BANG] = {unary, NULL, PREC_NONE},
  [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQ},
  [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQ},
  [TOKEN_GREATER] = {NULL, binary, PREC_COMP},
  [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMP},
  [TOKEN_LESS] = {NULL, binary, PREC_COMP},
  [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMP},
  [TOKEN_ID] = {variable, NULL, PREC_NONE},
  [TOKEN_STRING] = {string, NULL, PREC_NONE},
  [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
  [TOKEN_AND] = {NULL, NULL, PREC_NONE},
  [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
  [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
  [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
  [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
  [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
  [TOKEN_IF] = {NULL, NULL, PREC_NONE},
  [TOKEN_NIL] = {literal, NULL, PREC_NONE},
  [TOKEN_OR] = {NULL, NULL, PREC_NONE},
  [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
  [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
  [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
  [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
  [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
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

  bool canAssign = prec <= PREC_ASSIGN;
  prefixRule(canAssign);

  while (prec <= getRule(parser.curr.type)->prec) {
    advance();
    ParseFn infixRule = getRule(parser.prev.type)->infix;
    infixRule(canAssign);
  }

  if (canAssign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target");
  }
}

static ParseRule* getRule(TokenType type) {
  return &rules[type];
} 

static void expression() {
  parsePrec(PREC_ASSIGN);
}

static void varDecl() {
  uint8_t global = parseVar("Expected variable name");

  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON, "Expected' ';' in variable declaration");
  defineVar(global);
}

static void exprStmt() {
  expression();
  consume(TOKEN_SEMICOLON, "Expected ';' in expression statement");
  emitByte(OP_POP);
}

static void printStmt() {
  expression();
  consume(TOKEN_SEMICOLON, "Expected ';' in print statement");
  emitByte(OP_PRINT);
}

static void synchronize() {
  parser.panicMode = false;

  while (parser.curr.type != TOKEN_EOF) {
    if (parser.prev.type == TOKEN_SEMICOLON) {
      return;
    }
    switch (parser.curr.type) {
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;
      default: ;
    }
    advance();
  }
}

static void declaration() {
  if (match(TOKEN_VAR)) {
    varDecl();
  } else {
    statement();
  }
  if (parser.panicMode) {
    synchronize();
  }
}

static void statement() {
  if (match(TOKEN_PRINT)) {
    printStmt();
  } else {
    exprStmt();
  }
}

bool compile(const char* source, Chunk* chunk) {
  initScanner(source);
  compilingChunk = chunk;

  parser.hadErr = false;
  parser.panicMode = false;

  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }
  endCompiler();
  return !parser.hadErr;
}
