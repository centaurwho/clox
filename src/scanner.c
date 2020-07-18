#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char* start;
  const char* curr;
  int line;
} Scanner;

// Global scanner
Scanner scanner;

void initScanner(const char* source) {
  scanner.start = source;
  scanner.curr = source;
  scanner.line = 1;
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||  (c >= 'A' && c <= 'Z') || c == '_';
} 

static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

static bool isAtEnd() {
  return *scanner.curr == '\0';
}

static char advance() {
  scanner.curr++;
  return scanner.curr[-1];
}

static char peek() {
  return *scanner.curr;
}

static char peekNext() {
  if (isAtEnd()) {
    return '\0';
  }
  return scanner.curr[1];
}

static bool match(char exp) {
  if (isAtEnd() || (*scanner.curr != exp)) {
    return false;
  }
  scanner.curr++;
  return true;
}

static Token makeToken(TokenType type) {
  Token tok;
  tok.type = type;
  tok.start = scanner.start;
  tok.len = (int)(scanner.curr - scanner.start);
  tok.line = scanner.line;

  return tok;
}

static Token errorToken(const char* errMsg) {
  Token tok;
  tok.type = TOKEN_ERROR;
  tok.start = errMsg;
  tok.len = (int)(strlen(errMsg));
  tok.line = scanner.line;

  return tok;
}

void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch(c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
      case '\n':
        scanner.line++;
        advance();
        break;
      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) {
            advance();
          }
        } else {
          return;
        }
      default:
        return;
    }
  }
}

static TokenType checkKeyword(int start, int len, const char* rest, 
    TokenType type) {
  
  if (scanner.curr - scanner.start == start + len &&
      memcmp(scanner.start + start, rest, len) == 0) {
    return type;
  }
  return TOKEN_ID;
}

static TokenType identifierType() {
  // TODO: Why not just a trie. Convert to trie and run benchmarks
  switch(scanner.start[0]) {
    case 'a':
      return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c':
      return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
      return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if (scanner.curr - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'a':
            return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o':
            return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'u':
            return checkKeyword(2, 1, "n", TOKEN_FUN);
        }
      }
    case 'i':
      return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n':
      return checkKeyword(1, 2, "ot", TOKEN_NOT);
    case 'o':
      return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
      return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
      return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
      return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
      if (scanner.curr - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'h':
            return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r':
            return checkKeyword(2, 2, "ue", TOKEN_TRUE);
        }
      }
    case 'v':
      return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w':
      return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  }
  
  return TOKEN_ID;
}

static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek())) {
    advance();
  }
  return makeToken(identifierType());
}

static Token number() {
  while (isDigit(peek())) {
    advance();
  }

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TOKEN_NUMBER);
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      scanner.line++;
    }
    advance();
  }

  if (isAtEnd()) {
    return errorToken("Unterminated string.");
  }

  // end quotes
  advance();
  return makeToken(TOKEN_STRING);
}



Token scanToken() {
  skipWhitespace();
  scanner.start = scanner.curr;
  if (isAtEnd()) {
    return makeToken(TOKEN_EOF);
  }
  char c = advance();
  if (isDigit(c)) {
    return number();
  }
  if (isAlpha(c)) {
    return identifier();
  }
  switch (c) {
    case '(':
      return makeToken(TOKEN_LEFT_PAREN);
    case ')':
      return makeToken(TOKEN_RIGHT_PAREN);
    case '{':
      return makeToken(TOKEN_LEFT_BRACE);
    case '}':
      return makeToken(TOKEN_RIGHT_BRACE);
    case ';':
      return makeToken(TOKEN_SEMICOLON);
    case ',':
      return makeToken(TOKEN_COMMA);
    case '.':
      return makeToken(TOKEN_DOT);
    case '-':
      return makeToken(TOKEN_MINUS);
    case '+':
      return makeToken(TOKEN_PLUS);
    case '/':
      return makeToken(TOKEN_SLASH);
    case '*':
      return makeToken(TOKEN_STAR);
    case '!':
      return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
      return string();
  }

  return errorToken("Unexpected character");
  
}
