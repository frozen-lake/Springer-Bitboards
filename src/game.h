#include "board.h"
#include "attack_data.h"


#ifndef GAME_H

#define DEBUG 1

typedef struct Game Game;

struct Game {
	Board* board;
	int alive;
	
	AttackData* attack_data;
};


#endif


Game* create_game();
void destroy_game(Game* game);

void initialize_game(Game* game);
int load_fen(Game* game, char* str);


