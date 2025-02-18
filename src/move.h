#include "game.h"

#ifndef MOVE_H
#define MOVE_H

typedef struct Move Move;

struct Move {
    Move* prev;
    Move* next;
};

#endif

int parse_square(char* square);
int find_source_square(Board *board, char piece, int destination, char file_hint, int rank_hint);
int parse_algebraic_move(char* input, Board *board);
