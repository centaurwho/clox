#include <stdio.h>
#include "common.h"
#include "compiler.h"

#include "scanner.h"

void compile(const char* source) {
  initScanner(source);
  int line = -1;

  for (;;) {
    Token tok = scanToken();
    if (tok.line != line) {
      printf("%4d ", tok.line);
    } else {
      printf("   |");
    }
    printf("%2d '%.*s'\n", tok.type, tok.len, tok.start);
    if (tok.type == TOKEN_EOF) {
      break;
    }
  }
}
