#include "board.h"


#ifndef GAME_H

struct Game {
	Board* board;
	int alive;
	
	unsigned long long legal_to[64];
	unsigned long long legal_from[64]; 
};


#endif

typedef struct Game Game;

int load_fen(Game* game, char* str);
int verify_move(Game* game, unsigned int move);

