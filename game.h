#include "board.h"

typedef struct Board Board;

#ifndef GAME_H

struct Game {
	Board board;
	bool active;
	bool winner;
};


#endif

typedef struct Game Game;

bool verifyMove(Game game, unsigned int move);
int promptMove(Game game);


