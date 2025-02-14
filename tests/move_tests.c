#include <stdio.h>
#include <stdlib.h>
#include "../src/board.h"
#include "../src/move.h"
#include "tests.h"


int test_parse_square(){
	Board* board = create_board();
	int f3 = parse_square("f3");
	
	destroy_board(board);
	return f3 == 21;
}

int test_find_source_square(){
	Board* board = create_board();

	/* replace with generate_moves once that routine is functional */
	board->attack_to[6] |= (1 << 21);
	board->attack_to[12] |= (1 << 28);

	int Nf3_src = find_source_square(board, 'N', 21, 0, -1);
	int e4_src = find_source_square(board, 'P', 28, 0, -1);
	
	// printf("Nf3src: %d, e4src: %d\n", Nf3_src, e4_src);
	
	destroy_board(board);
	return Nf3_src == 6 && e4_src == 12;
}

int test_parse_algebraic_move(){
	
	Board* board = create_board();
	char* move = "Nf3";
	
	/* replace with generate_moves once that routine is functional */
	board->attack_to[6] |= (1 << 21);
	board->attack_to[12] |= (1 << 28);

	int Nf3 = parse_algebraic_move(move, board);
	int e4 = parse_algebraic_move("e4", board);
	
	// printf("Encoded move: %d\n", e4);
	
	destroy_board(board);
	return Nf3 == (6 << 6) + 21 && e4 == (12 << 6) + 28;
}

int test_generate_rook_moves(){
	Game* game = create_game();

	load_fen(game, "4k3/8/8/1n2p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");

	// print_board(game->board);

	generate_rook_moves(game, 18);
	int success = game->legal_from[18] == 0b10000000100000001000000010000000100111110110000010000000100ULL;


	destroy_game(game);
	return 0;
}


void move_tests(){

	int num_tests = 4;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_parse_square;
	test_cases[1] = test_find_source_square;
	test_cases[2] = test_parse_algebraic_move;
	test_cases[3] = test_generate_rook_moves;

	test_case_names[0] = "test_parse_square";
	test_case_names[1] = "test_find_source_square";
	test_case_names[2] = "test_parse_algebraic_move";
	test_case_names[3] = "test_generate_rook_moves";

    run_tests(test_cases, test_case_names, num_tests);

}