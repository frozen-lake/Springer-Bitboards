#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "tests.h"
#include "generate_moves.h"




void start_sandbox(){
	
	Board* board = malloc(sizeof(Board));
	initialize_board(board);
 
	print_board(board);
	
	generate_moves(board);
	
	bool game_loop = 1;
	while(game_loop){
		generate_moves(board);
		// prompt_move();
	}

	free(board);

}

int main() {
	
	
	
	start_sandbox();

	return 0;
}
