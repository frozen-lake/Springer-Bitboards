#include <stdlib.h>
#include <string.h>
#include "../src/transposition_table.h"


void table_add(TranspositionTable* table, uint64_t key, Move move, int score, int depth, int flag){
    int index = key & (table->size - 1);

    TranspositionTableEntry* entry = &table->entries[index];
    if((entry->key == 0) || (entry->depth <= depth)){
        entry->key = key;
        entry->score = score;
        entry->depth = depth;
        entry->best_move = move;
        entry->flag = flag;
    }
}

TranspositionTableEntry* table_get(TranspositionTable* table, uint64_t key){
    int index = key & (table->size - 1);
    TranspositionTableEntry* entry = &table->entries[index];
    
    if(entry->key == key){
        return entry;
    }
    return NULL;
}

/* Set all fields of all entries to 0 */
void table_clear(TranspositionTable* table){
    memset(table->entries, 0, (table->size * sizeof(TranspositionTableEntry)));
}

/* Allocate n megabytes of memory to this table */
void table_init(TranspositionTable* table){
    table->size = TT_ENTRIES;
}
