#include "game.h"

#ifndef TTABLE_H
#define TTABLE_H
#define TT_ENTRIES 1048576 // ~129 MB

typedef struct {
    uint64_t key;
    Move best_move;
    int16_t score;
    uint8_t depth;
    uint8_t flag;
} TranspositionTableEntry;

typedef struct {
    TranspositionTableEntry* entries;
    uint8_t* locks;
    int size;
} TranspositionTable;

#endif

void table_add(TranspositionTable* table, uint64_t key, Move move, int score, int depth, int flag);
void table_clear(TranspositionTable* table);
TranspositionTableEntry* table_get(TranspositionTable* table, uint64_t key);
void table_init(TranspositionTable* table);
