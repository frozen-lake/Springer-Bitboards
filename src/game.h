#include "board.h"
#include "attack_data.h"


#ifndef GAME_H
#define GAME_H

#define DEBUG 1

typedef struct Game Game;

struct Game {
	Board* board;
	int alive;

	/* 1 bit for white kingside, 1 bit for white queenside
	 * 1 bit for black kingside, 1 bit for black queenside */
	uint8_t castling;
};


#endif


Game* create_game();
void destroy_game(Game* game);

void initialize_game(Game* game);
int load_fen(Game* game, char* str);

/* Utilities */
int get_lsb_index(uint64_t num);
uint64_t swap_uint64(uint64_t num);

