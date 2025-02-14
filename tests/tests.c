#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "move_tests.h"
#include "../src/move.h"
#include "../src/attack_data.h"


int test_load_fen(){
	Game* game = create_game();
	
	char* fen = "4k3/8/8/1n2p3/4P1Pp/8/8/3BK3 b - g3 0 1";

	int success = load_fen(game, fen);


	Board* board = game->board;
	
	if(!success){
		destroy_game(game);
		return 0;
	}

	int pawn_match = (board->pieces[Pawn]) == ((1ULL << 28) | (1ULL << 36) | (1ULL << 30) | (1ULL << 31));
	
	int bad_fen = load_fen(game, "4k3/8/8/1n3p3/4P1Pp/8/8/3BK3 b - g3 0 1");

	destroy_game(game);
	return pawn_match && !bad_fen;
}

int test_attack_data(){
	AttackData* attack_data = create_attack_data();

    int success = attack_data->bishop[12] == 0b11000001001000100001010000000000000101000;
    success = success && attack_data->rook[12] == 0b1000000010000000100000001000000010000000100001110111100010000;
    success = success && attack_data->queen[20] == 0b1000000010001100100100101010000111000111011110011100001010100;

	destroy_attack_data(attack_data);
	return success;
};

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
	int num_tests = 2;

	int (*test_cases[num_tests])(); // array of function pointers
	char* test_case_names[num_tests];

	test_cases[0] = test_load_fen;
	test_cases[1] = test_attack_data;

	test_case_names[0] = "test_load_fen";
	test_case_names[1] = "test_attack_data";


	printf("====== GAME TESTS ======\n");
	run_tests(test_cases, test_case_names, num_tests);

	printf("====== MOVE TESTS ======\n");
	move_tests();

	return 0;
}
