#include "game.h"

#ifndef MOVE_H
#define MOVE_H

typedef struct Move Move;

struct Move {
    Move* prev;
    Move* next;
};

#endif

int parse_square(const char *square);
int find_source_square(const Board *board, char piece, int destination, char file_hint, int rank_hint);
int parse_algebraic_move(char* input, const Board *board);

void generate_rook_moves(Game* game, int origin);
void generate_all_rook_moves(Game* game);
