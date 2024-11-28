#include "board.h"


int parse_square(const char *square);
int find_source_square(const Board *board, char piece, int destination, char file_hint, int rank_hint);
int parse_algebraic_move(char* input, const Board *board);
