#include <stdio.h>
#include <stdlib.h>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/game.h"
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

	int Nf3_src = find_source_square(board, 'N', 21, 0, -1);
	int e4_src = find_source_square(board, 'P', 28, 0, -1);
	
	// printf("Nf3src: %d, e4src: %d\n", Nf3_src, e4_src);
	
	destroy_board(board);
	return Nf3_src == 6 && e4_src == 12;
}

int test_find_source_square2(){
	Game* game = create_game();
	initialize_game(game);
	Board* board = game->board;


	Move e4 = encode_move(12, 28, board);
	make_move(game, e4);

	int Nf3_src = find_source_square(board, 'N', 21, 0, -1);


	return Nf3_src == 6;
}

int test_parse_algebraic_move(){
	Game* game = create_game();
	initialize_game(game);
	Board* board = game->board;

	Move Nf3 = parse_algebraic_move("Nf3", game);
	Move e4 = parse_algebraic_move("e4", game);
	
	destroy_board(board);
	return Nf3 == 0x31546 && e4 == 0x3070c;
}


int test_populate_rook_attack(){
	/* Test single square rook attack population */
	Game* game = create_game();
	int success = 1;

	/* Rook on c3 with no blockers */
	load_fen(game, "4k3/8/8/1n2p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_rook_attack(game->board, 18);
	success = (game->board->attack_from[18] == 0b10000000100000001000000010000000100111110110000010000000100ULL);
	
	/* Rook on c5 with no blockers */
	load_fen(game, "4k3/8/8/2R5/4P1Pp/8/8/3BK3 b - g3 0 1");
	populate_rook_attack(game->board, 34);
	success = (game->board->attack_from[34] == 0b10000000100000001001111101100000100000001000000010000000100ULL);

	/* Rook on c3 with enemy blocker on e3 and friendly blocker on c6 */
	load_fen(game, "5k2/8/2Nr1p2/8/8/2R1b3/8/5K2 w - - 0 1");
	populate_rook_attack(game->board, 18);
	success = success && (game->board->attack_from[18] == 0b1000000010000000100000110110000010000000100);

	destroy_game(game);
	return success;
}

int test_populate_bishop_attack(){
	/* Test single square bishop attack population */
	Game* game = create_game();

	/* Single bishop on c5 with no blockers */
	load_fen(game, "4k3/8/8/1nb1p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_bishop_attack(game->board, 34);
	int success = game->board->attack_from[34] == 0b10000000010001000010100000000000001010000100010010000001000000ULL;
	/* Single bishop on f4 with a friendly blocker on c7 and an enemy blocker on d2 */
	load_fen(game, "5k2/2p5/8/8/5b2/8/3N4/5K2 b - - 0 1");
	populate_bishop_attack(game->board, 29);
	success = game->board->attack_from[29] == 0b100100010000101000000000000010100001000100000000000;

	destroy_game(game);
	return success;
}

int test_populate_queen_attack(){
	/* Test single square queen attack population */
	Game* game = create_game();

	/* Single queen on c5 with no blockers */
	load_fen(game, "4k3/8/8/2Q5/4P1Pp/8/8/3BK3 b - g3 0 1");
	populate_queen_attack(game->board, 34);
	int success = game->board->attack_from[34] == 0b10010000010101000011101111101100001110000101010010010001000100ULL;

	/* Single queen on c5 with a friendly blocker on e3 and an enemy blocker on e7 */
	load_fen(game, "4k3/4p3/8/2Q5/4P1Pp/4N3/8/3BK3 b - g3 0 1");
	populate_queen_attack(game->board, 34);
	success = game->board->attack_from[34] == 0b10000010101000011101111101100001110000101010000010000000100ULL;

	destroy_game(game);
	return success;
}

int test_populate_knight_attack(){
	Game* game = create_game();

	destroy_game(game);

	return 0;
}
int test_populate_pawn_attack(){
	int success =  attack_data.pawn_black[28] == (U64_MASK(19) | U64_MASK(21));
	success = success && attack_data.pawn_white[27] == (U64_MASK(34) | U64_MASK(36));
	return success;
}
int test_populate_king_attack(){
	return 0;
}

int test_generate_occupancy_table(){

	int success = attack_data.occupancy_table[4][0b101001] == 0b1101110;
	success = success && attack_data.occupancy_table[6][0b101001] == 0b10110000;
	success = success && attack_data.occupancy_table[0][0b111111] == 0b00000010;
	success = success && attack_data.occupancy_table[2][0b111111] == 0b00001010;

	return success;
}

void move_tests(){
	int num_tests = 4;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_parse_square;
	test_cases[1] = test_find_source_square;
	test_cases[2] = test_find_source_square2;
	test_cases[3] = test_parse_algebraic_move;

	test_case_names[0] = "test_parse_square";
	test_case_names[1] = "test_find_source_square";
	test_case_names[2] = "test_find_source_square2";
	test_case_names[3] = "test_parse_algebraic_move";

    run_tests(test_cases, test_case_names, num_tests);

}

void attack_tests(){
	
	int num_tests = 5;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_populate_rook_attack;
	test_cases[1] = test_populate_bishop_attack;
	test_cases[2] = test_populate_queen_attack;
	test_cases[3] = test_populate_pawn_attack;
	test_cases[4] = test_generate_occupancy_table;

	test_case_names[0] = "test_populate_rook_attack";
	test_case_names[1] = "test_populate_bishop_attack";
	test_case_names[2] = "test_populate_queen_attack";
	test_case_names[3] = "test_populate_pawn_attack";
	test_case_names[4] = "test_generate_occupancy_table";

    run_tests(test_cases, test_case_names, num_tests);
}
