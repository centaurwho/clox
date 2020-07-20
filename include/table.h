#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjStr* key;
  Value val;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool addEntry(Table* table, ObjStr* key, Value val);
bool getEntry(Table* table, ObjStr* key, Value* val);
bool delEntry(Table* table, ObjStr* key);
void copyTable(Table* from, Table* to);

#endif
