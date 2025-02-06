#include "board.h"


#ifndef GAME_H

struct Game {
	Board* board;
	int alive;
	
	uint64_t legal_to[64];
	uint64_t legal_from[64]; 
};


#endif

typedef struct Game Game;

Game* create_game();
void destroy_game(Game* game);

void initialize_game(Game* game);
int load_fen(Game* game, char* str);


