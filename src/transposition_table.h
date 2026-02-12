#include "game.h"

#ifndef TTABLE_H
#define TTABLE_H
#define TT_ENTRIES 1048576 // 2^20

// Transposition table entry flags
#define TT_EXACT 0
#define TT_LOWER 1
#define TT_UPPER 2

typedef struct {
    uint64_t key;
    Move best_move;
    int16_t score;
    uint8_t depth;
    uint8_t flag;
    uint8_t age;
} TranspositionTableEntry;

typedef struct {
    TranspositionTableEntry* entries;
    int size;
    uint8_t age;
} TranspositionTable;

#endif

void table_add(TranspositionTable* table, uint64_t key, Move move, int score, int depth, int flag);
void table_clear(TranspositionTable* table);
TranspositionTableEntry* table_get(TranspositionTable* table, uint64_t key);
void table_init(TranspositionTable* table);
void table_free(TranspositionTable* table);
