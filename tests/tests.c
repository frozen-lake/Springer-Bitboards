#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "attack_and_move_tests.h"
#include "move_gen_tests.h"
#include "../src/game.h"
#include "../src/move.h"
#include "../src/attack_data.h"
#include "../src/move_gen.h"

int test_game_init(){
	Game* game = create_game();
	initialize_game(game);

	int success = game->board->attack_from[G1] & U64_MASK(21);

	destroy_game(game);
	return success;
}

int test_load_fen(){
	/* Nominal case */
	Game* game = create_game();
	
	char* fen = "4k3/8/8/1n2p3/4P1Pp/8/8/3BK3 b - g3 0 1";
	int success = load_fen(game, fen);
	success = !game->side_to_move;
	
	if(!success){
		destroy_game(game);
		return 0;
	}

	success = success && (game->board->pieces[Pawn]) == (U64_MASK(E4) | U64_MASK(E5) | U64_MASK(G4) | U64_MASK(H4));
	success = success && (game->board->attack_from[B5] == (uint64_t) 0b101000010000000000000001000000001010000000000000000);
	success = success && (game->board->pieces[White] == 0b1010000000000000000000000011000);
	success = success && (game->board->pieces[Black] == 0b1000000000000000000000001001010000000000000000000000000000000);
	
	/* Bad FEN should return -1 */
	success = success && !load_fen(game, "4k3/8/8/1n3p3/4P1Pp/8/8/3BK3 b - g3 0 1");


	destroy_game(game);
	return success;
}

int test_make_move(){
	Game* game = create_game();
	
	char* fen = "4k3/8/8/1n2p3/4P1Pp/2P5/8/3BK3 b - g3 0 1";
	int success = load_fen(game, fen);

	Move move = B5 | (C3 << 6) | (Knight << 12) | (Pawn << 15); // 
	make_move(game, move);

	/* Piece is off of source square */
	success = success && ((game->board->pieces[Knight] & U64_MASK(B5)) == 0);
	success = success && ((game->board->pieces[Black] & U64_MASK(B5)) == 0);

	/* Piece is on destination square */
	success = success && (game->board->pieces[Knight] & U64_MASK(C3));
	success = success && (game->board->pieces[Black] & U64_MASK(C3));

	/* Captured piece is gone */
	success = success && ((game->board->pieces[Pawn] & U64_MASK(C3)) == 0);
	success = success && ((game->board->pieces[White] & U64_MASK(C3)) == 0);

	destroy_game(game);
	return success;
}

int run_tests(int (*test_cases[])(), char** test_case_names, int num_cases){
	int result = 1;
	for(int i=0;i<num_cases;i++){
		if(!test_cases[i]()){
			fprintf(stderr, "[x] FAIL: %s\n", test_case_names[i]);
			result = 0;
		} else {
			printf("[ ] PASS: %s\n", test_case_names[i]);
		}
	}
	return result;
}

int main(){
	initialize_attack_data();


	int num_tests = 3;

	int (*test_cases[num_tests])(); // array of function pointers
	char* test_case_names[num_tests];

	test_cases[0] = test_load_fen;
	test_cases[1] = test_make_move;
	test_cases[2] = test_game_init;

	test_case_names[0] = "test_load_fen";
	test_case_names[1] = "test_make_move";
	test_case_names[2] = "test_game_init";


	printf("====== GAME TESTS ======\n");
	run_tests(test_cases, test_case_names, num_tests);

	printf("====== MOVE TESTS ======\n");
	move_tests();

	printf("====== ATTACK TESTS ======\n");
	attack_tests();

	printf("====== MOVE GEN TESTS ======\n");
	move_gen_tests();

	printf("======\n");
	

	return 0;
}
