#include "board.h"


#ifndef GAME_H

struct Game {
	Board board;
	bool active;
	bool winner;
};


#endif

typedef struct Game Game;

bool verify_move(Game game, unsigned int move);
int prompt_move(Game game);


