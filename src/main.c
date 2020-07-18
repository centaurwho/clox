#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "debug.h"
#include "vm.h"

static void repl() {
  char line[1024];

  for (;;) {
    printf("clox > ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }
  }
}

static char* readFile(const char* filepath) {
  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file '%s'", filepath);
    exit(74);
  }

  // Get size of file
  fseek(file, 0L, SEEK_END);
  size_t sz = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(sz + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read '%s'", filepath);
    exit(74);
  }

  size_t read = fread(buffer, sizeof(char), sz, file);
  if (read < sz) {
    fprintf(stderr, "Could not read file '%s'", filepath);
    exit(74);
  }

  // terminating null byte
  buffer[read] = '\0';
  
  fclose(file);
  return buffer;
}

static void runFile(const char* filepath) {
  char* source = readFile(filepath);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) {
    exit(65);
  }
  if (result == INTERPRET_RUNTIME_ERROR) {
    exit(70);
  }
}

int main(int argc, const char* argv[]) {
  initVM();

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clox <path>\n");
    exit(64);
  }

  freeVM();

  return 0;
}
