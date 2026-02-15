#include "board.h"
#include "attack_data.h"
#include "move.h"

#ifndef GAME_H
#define GAME_H

#define DEBUG_ERR 1
#define DEBUG_PERF 1

typedef struct Game Game;

struct Game {
	BoardState state;
	MoveList legal_moves;
	Move* move_history;
	int move_history_capacity;
};


#endif


Game* create_game();
void destroy_game(Game* game);
void initialize_game(Game* game);
int load_fen(Game* game, char* str);

void make_move(Game* game, Move move);
void unmake_move(Game* game, Move move, uint8_t depth);


/* Utilities */
int get_lsb_index(uint64_t num);
uint64_t swap_uint64(uint64_t num);

