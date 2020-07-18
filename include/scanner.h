#ifndef clox_scanner_h
#define clox_scanner_h

typedef enum {
  // Single char tokens
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

  // One or two char tokens
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,

  // literals
  TOKEN_ID, TOKEN_STRING, TOKEN_NUMBER,

  // keywords
  TOKEN_AND, TOKEN_OR, TOKEN_NOT, 
  TOKEN_IF, TOKEN_ELSE, TOKEN_FOR, TOKEN_WHILE,
  TOKEN_FALSE, TOKEN_TRUE,
  TOKEN_CLASS, TOKEN_SUPER, TOKEN_THIS, 
  TOKEN_PRINT, TOKEN_RETURN, TOKEN_FUN,
  TOKEN_NIL, TOKEN_VAR,

  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;



typedef struct {
  TokenType type;
  const char* start;
  int len;
  int line; // TODO: inefficient to keep line number of every token
} Token;

void initScanner(const char* source);
Token scanToken();

#endif

