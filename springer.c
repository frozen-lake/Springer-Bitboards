#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"

typedef struct Board Board;


void startSandbox(){
	
	Board* board = malloc(sizeof(Board));
	initializeBoard(board);
 
	printBoard(board);
	
	bool gameLoop = 1;
	while(gameLoop){
		// promptMove();
	}

}

int main() {
	
	startSandbox();

	return 0;
}
