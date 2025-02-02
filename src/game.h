#include "board.h"


#ifndef GAME_H

struct Game {
	Board* board;
	bool alive;
	
	unsigned long long legal_to[64];
	unsigned long long legal_from[64]; 
};


#endif

typedef struct Game Game;

bool verify_move(Game game, unsigned int move);
int prompt_move(Game game);


