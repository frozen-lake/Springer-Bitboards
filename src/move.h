#include "game.h"

#ifndef MOVE_H
#define MOVE_H

#define MAX_MOVES 218

typedef uint32_t Move;

/*
 * Move encoding:
 * 6 bits - source square
 * 6 bits - destination square
 * 3 bits - moved piece
 * 3 bits - captured piece
 * 3 bits - promotion: (0=No promotion, 1=Knight, 2=Bishop, 3=Rook, 4=Queen)
 * 2 bits - special move flags
*/


typedef struct MoveList MoveList;

struct MoveList {
    Move moves[MAX_MOVES];
    int size;
};

#endif

int parse_square(char* square);
int find_source_square(Board *board, char piece, int destination, char file_hint, int rank_hint);
int parse_algebraic_move(char* input, Board *board);

void print_move(Move move);
void print_moves(MoveList* move_list);

void move_list_add(MoveList* move_list, Move move);
void move_list_clear(MoveList* move_list);

Move encode_move(int src, int dest, Board* board);
Move encode_promotion(int src, int dest, Board* board, int promotion);