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
	initialize_board(board);

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
	initialize_board(board);
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

int test_populate_rook_attack(){
	/* Test single square rook attack population */
	Game* game = create_game();
	int success;

	/* Rook on c3 with no blockers */
	load_fen(game, "4k3/8/8/1n2p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_rook_attack(game->board, game->attack_data, 18);
	success = (game->board->attack_from[18] == 0b10000000100000001000000010000000100111110110000010000000100ULL)
				  && (game->board->attack_to[2] == game->board->attack_to[22] == 0b1000000000000000000ULL);

	/* Rook on c3 with enemy blocker on e3 and friendly blocker on c6 */
	load_fen(game, "5k2/8/2Nr1p2/8/8/2R1b3/8/5K2 w - - 0 1");
	populate_rook_attack(game->board, game->attack_data, 18);
	success = success && (game->board->attack_from[18] == 0b10000000100000110110000010000000100ULL);

	destroy_game(game);
	return 0;
}

int test_populate_bishop_attack(){
	/* Test single square bishop attack population */
	Game* game = create_game();

	/* Single bishop on c5 with no blockers */
	load_fen(game, "4k3/8/8/1nb1p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_bishop_attack(game->board, game->attack_data, 18);
	int success = game->board->attack_from[34] == 0b10000000010000000010000000000000001010000100010010000001000000ULL;

	/* Single bishop on e4 with a friendly blocker on b7 and an enemy blocker on g6 */
	load_fen(game, "5k2/1p6/6P1/8/4b3/8/8/5K2 b - - 0 1");
	populate_bishop_attack(game->board, game->attack_data, 28);
	success = game->board->attack_from[28] == 0b10001000010100000000000001010000100010010000010;

	destroy_game(game);
	return 0;
}

int test_populate_queen_attack(){
	/* Test single square queen attack population */
	Game* game = create_game();

	/* Single queen on c5 with no blockers */
	load_fen(game, "4k3/8/8/2Q5/4P1Pp/8/8/3BK3 b - g3 0 1");
	populate_queen_attack(game->board, game->attack_data, 34);
	int success = game->board->attack_from[34] == 0b10010000010101000010101111101100001110000101010010010001000100ULL;

	/* Single queen on c5 with a friendly blocker on e3 and an enemy blocker on e7 */
	load_fen(game, "4k3/4p3/8/2Q5/4P1Pp/4N3/8/3BK3 b - g3 0 1");
	populate_queen_attack(game->board, game->attack_data, 34);
	success = game->board->attack_from[34] == 0b10101000011101111101100001110000001010000010000000100ULL;

	destroy_game(game);
	return 0;
}

void move_tests(){
	int num_tests = 3;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_parse_square;
	test_cases[1] = test_find_source_square;
	test_cases[2] = test_parse_algebraic_move;

	test_case_names[0] = "test_parse_square";
	test_case_names[1] = "test_find_source_square";
	test_case_names[2] = "test_parse_algebraic_move";

    run_tests(test_cases, test_case_names, num_tests);

}

void attack_tests(){
	
	int num_tests = 3;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_populate_rook_attack;
	test_cases[1] = test_populate_bishop_attack;
	test_cases[2] = test_populate_queen_attack;

	test_case_names[0] = "test_populate_rook_attack";
	test_case_names[1] = "test_populate_bishop_attack";
	test_case_names[2] = "test_populate_queen_attack";

    run_tests(test_cases, test_case_names, num_tests);
}
