#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

static Entry* findEntry(Entry* entries, int cap, ObjStr* key) {
  uint32_t index = key->hash % cap;
  Entry* tombstone = NULL;
  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->val)) {
        return tombstone != NULL ? tombstone : entry;
      } else if (tombstone == NULL) {
        tombstone = entry;
      }
    }

    if (entry->key == key) {
      return entry;
    }

    index = (index + 1) % cap;
  }
}

static void adjustCap(Table* table, int cap) {
  Entry* entries = ALLOCATE(Entry, cap);
  for (int i = 0; i < cap; i++) {
    entries[i].key = NULL;
    entries[i].val = NIL_VAL;
  }
  // Recalculate hash values and insert entries into this new array
  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) {
      continue;
    }
    Entry* dest = findEntry(entries, cap, entry->key);
    dest->key = entry->key;
    dest->val = entry->val;
    table->count++;
  }
  FREE_ARRAY(Entry, table->entries, table->capacity);

  table->entries = entries;
  table->capacity = cap;
}

bool addEntry(Table* table, ObjStr* key, Value val) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int cap = GROW_CAPACITY(table->capacity);
    adjustCap(table, cap);
  }
  Entry* entry = findEntry(table->entries, table->capacity, key);

  bool isNew = entry->key == NULL;
  if (!isNew && IS_NIL(entry->val)) {
    table->count++;
  }
  if (isNew) {
    table->count++;
  }

  entry->key = key;
  entry->val = val;
  return isNew;
}

void copyTable(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      addEntry(to, entry->key, entry->val);
    }
  }
}

bool getEntry(Table* table, ObjStr* key, Value* val) {
  if (table->count == 0) {
    return false;
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) {
    return false;
  }
  *val = entry->val;
  return true;
}

bool delEntry(Table* table, ObjStr* key) {
  if (table->count == 0) {
    return false;
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) {
    return false;
  }

  entry->key = NULL;
  entry->val = BOOL_VAL(true);
  return true;
}

ObjStr* tableFindStr(Table* table, const char* chars, int len, 
    uint32_t hash) {
  if (table->count == 0) {
    return NULL;
  }
  uint32_t index = hash % table->capacity;
  for (;;) {
    Entry* entry = &table->entries[index];

    if (entry->key == NULL) {
      if (IS_NIL(entry->val)) {
        return NULL;
      }
    } else if (entry->key->len == len && entry->key->hash == hash &&
        memcmp(entry->key->chars, chars, len) == 0) {
      return entry->key; 
    }
    index = (index + 1) % table->capacity;
  }
}
