#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "attack_data_tests.h"
#include "move_tests.h"
#include "../src/game.h"
#include "../src/move.h"
#include "../src/attack_data.h"



// ====== board tests

int test_load_fen(){
	Game* game = create_game();
	
	char* fen = "4k3/8/8/1n2p3/4P1Pp/8/8/3BK3 b - g3 0 1";
	int success = load_fen(game, fen);
	Board* board = game->board;
	
	// printf("Board after load_fen:\n");
	// print_board(board);
	
	if(!success){ destroy_game(game); return 0; }
	
	if(!board){ destroy_game(game); return 0; }

	int pawn_match = (board->pieces[Pawn]) == ((1ULL << 28) | (1ULL << 36) | (1ULL << 30) | (1ULL << 31));

	// To-Do: test en passant square and turn
	
	int bad_fen = load_fen(game, "4k3/8/8/1n3p3/4P1Pp/8/8/3BK3 b - g3 0 1");

	destroy_game(game);
	return pawn_match && !bad_fen;
}

int run_tests(int (*test_cases[])(), char** test_case_names, int num_cases){
	for(int i=0;i<num_cases;i++){
		if(!test_cases[i]()){
			fprintf(stderr, "Test FAILED: %s\n", test_case_names[i]);
		} else {
			printf("Test passed: %s\n", test_case_names[i]);
		}
	}
}

int main(){
	int num_tests = 1;

	int (*test_cases[num_tests])(); // array of function pointers
	char* test_case_names[num_tests];

	test_cases[0] = test_load_fen;

	test_case_names[0] = "test_load_fen";


	printf("====== MISC TESTS ======\n");
	run_tests(test_cases, test_case_names, num_tests);

	printf("====== ATTACK_DATA TESTS ======\n");
	attack_data_tests();

	printf("====== MOVE TESTS ======\n");
	move_tests();

	return 0;
}
