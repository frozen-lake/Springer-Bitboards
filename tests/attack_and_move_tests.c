#include <stdio.h>
#include <stdlib.h>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/game.h"
#include "tests.h"


int test_parse_square(){
	int f3 = parse_square("f3");
	return f3 == F3;
}

int test_find_source_square(){
	Board board = {0};
	initialize_board(&board);

	int Nf3_src = find_source_square(&board, 'N', F3, 0, -1);
	int e4_src = find_source_square(&board, 'P', E4, 0, -1);
	
	return Nf3_src == G1 && e4_src == E2;
}

int test_find_source_square2(){
	Game* game = create_game();
	initialize_game(game);
	Board* board = &game->state;


	Move e4 = encode_move(E2, E4, board);
	make_move(game, e4);

	int Nf3_src = find_source_square(board, 'N', F3, 0, -1);

	destroy_game(game);
	return Nf3_src == 6;
}

int test_parse_algebraic_move(){
	Game* game = create_game();
	initialize_game(game);
	Board* board = &game->state;

	Move Nf3 = parse_algebraic_move("Nf3", game);
	Move e4 = parse_algebraic_move("e4", game);
	
    int success = Nf3 == (G1 | (F3 << 6) | (Knight << 12));
    success = success && e4 == (E2 | (E4 << 6) | (Pawn << 12));

	destroy_game(game);
	return success;
}


int test_populate_rook_attack(){
	/* Test single square rook attack population */
	Game* game = create_game();
	int success = 1;

	/* Rook on c3 with no blockers */
	load_fen(game, "4k3/8/8/1n2p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_rook_attack(&game->state, C3);
	success = (game->state.attack_from[C3] == 0b10000000100000001000000010000000100111110110000010000000100ULL);
	
	/* Rook on c5 with no blockers */
	load_fen(game, "4k3/8/8/2R5/4P1Pp/8/8/3BK3 b - g3 0 1");
	populate_rook_attack(&game->state, C5);
	success = (game->state.attack_from[C5] == 0b10000000100000001001111101100000100000001000000010000000100ULL);

	/* Rook on c3 with enemy blocker on e3 and friendly blocker on c6 */
	load_fen(game, "5k2/8/2Nr1p2/8/8/2R1b3/8/5K2 w - - 0 1");
	populate_rook_attack(&game->state, C3);
	success = success && (game->state.attack_from[C3] == 0b1000000010000000100000110110000010000000100);

	destroy_game(game);
	return success;
}

int test_populate_bishop_attack(){
	/* Test single square bishop attack population */
	Game* game = create_game();

	/* Single bishop on c5 with no blockers */
	load_fen(game, "4k3/8/8/1nb1p3/4P1Pp/2R5/8/3BK3 b - g3 0 1");
	populate_bishop_attack(&game->state, C5);
	int success = game->state.attack_from[C5] == 0b10000000010001000010100000000000001010000100010010000001000000ULL;
	/* Single bishop on f4 with a friendly blocker on c7 and an enemy blocker on d2 */
	load_fen(game, "5k2/2p5/8/8/5b2/8/3N4/5K2 b - - 0 1");
	populate_bishop_attack(&game->state, F4);
	success = game->state.attack_from[F4] == 0b100100010000101000000000000010100001000100000000000;

	destroy_game(game);
	return success;
}

int test_populate_queen_attack(){
	/* Test single square queen attack population */
	Game* game = create_game();

	/* Single queen on c5 with no blockers */
	load_fen(game, "4k3/8/8/2Q5/4P1Pp/8/8/3BK3 b - g3 0 1");
	populate_queen_attack(&game->state, C5);
	int success = game->state.attack_from[C5] == 0b10010000010101000011101111101100001110000101010010010001000100ULL;

	/* Single queen on c5 with a friendly blocker on e3 and an enemy blocker on e7 */
	load_fen(game, "4k3/4p3/8/2Q5/4P1Pp/4N3/8/3BK3 b - g3 0 1");
	populate_queen_attack(&game->state, C5);
	success = game->state.attack_from[C5] == 0b10000010101000011101111101100001110000101010000010000000100ULL;

	destroy_game(game);
	return success;
}

int test_populate_pawn_attack(){
	int success =  attack_data.pawn_black[E4] == (U64_MASK(D3) | U64_MASK(F3));
	success = success && attack_data.pawn_white[D4] == (U64_MASK(C5) | U64_MASK(E5));
	return success;
}

int test_generate_occupancy_table(){

	int success = attack_data.occupancy_table[4][0b101001] == 0b1101110;
	success = success && attack_data.occupancy_table[6][0b101001] == 0b10110000;
	success = success && attack_data.occupancy_table[0][0b111111] == 0b00000010;
	success = success && attack_data.occupancy_table[2][0b111111] == 0b00001010;

	return success;
}

int test_square_attacked(){
	Game* game = create_game();
	load_fen(game, "4k3/4p3/5n2/2Q5/4P1Pp/4N3/8/3BK3 b - g3 0 1");

	int success = square_attacked(&game->state, E7, White);
	success = success && square_attacked(&game->state, E7, Black);

	success = success && square_attacked(&game->state, E4, Black);
	success = success && !square_attacked(&game->state, E4, White);

	success = success && !square_attacked(&game->state, C5, Black);
	success = success && !square_attacked(&game->state, C5, White);

	success = success && !square_attacked(&game->state, E1, Black);
	success = success && !square_attacked(&game->state, E1, White);
	
	success = success && square_attacked(&game->state, G4, Black);
	success = success && square_attacked(&game->state, G4, White);

	destroy_game(game);
	return success;
}

int test_is_legal_move(){
	Game* game = create_game();
	load_fen(game, "4k3/4p3/5n2/8/3QP1Pp/4N3/8/3BK3 b - g3 0 1");
	int illegal = is_legal_move(game, (E8 | (D7 << 6) | (King << 12)));
	int legal = is_legal_move(game, (E8 | (F7 << 6) | (King << 12)));

	return legal && !illegal;
}

int test_is_legal_move_en_passant(){
	Game* game = create_game();
	load_fen(game, "4k3/7b/8/4Pp2/8/8/2K5/8 w - f6 0 1");

	Move en_passant = E5 | (F6 << 6) | (Pawn << 12) | (Pawn << 15) | (EnPassant << 21);
	int legal = is_legal_move(game, en_passant);

	destroy_game(game);
	return !legal;
}

int test_is_legal_move_castling(){
	int success = 1;
	Move w_kingside = E1 | (G1 << 6) | (King << 12) | (Kingside << 21);
	Move b_queenside = E8 | (C8 << 6) | (King << 12) | (Queenside << 21);

	Game* game = create_game();
	load_fen(game, "4k3/8/8/8/8/8/8/R3K2R w K - 0 1");
	success = success && is_legal_move(game, w_kingside);
	destroy_game(game);

	game = create_game();
	load_fen(game, "4r3/8/8/8/8/8/8/R3K2R w K - 0 1");
	success = success && !is_legal_move(game, w_kingside);
	destroy_game(game);

	game = create_game();
	load_fen(game, "4k3/8/8/8/2b5/8/8/R3K2R w K - 0 1");
	success = success && !is_legal_move(game, w_kingside);
	destroy_game(game);

	game = create_game();
	load_fen(game, "4k3/8/8/8/8/8/8/R3KN1R w K - 0 1");
	success = success && !is_legal_move(game, w_kingside);
	destroy_game(game);

	game = create_game();
	load_fen(game, "r3k2r/8/8/8/8/8/8/8 b kq - 0 1");
	success = success && is_legal_move(game, b_queenside);
	destroy_game(game);

	return success;
}

int move_tests(){
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

    return run_tests(test_cases, test_case_names, num_tests);

}

int attack_tests(){
	
	int num_tests = 9;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_populate_rook_attack;
	test_cases[1] = test_populate_bishop_attack;
	test_cases[2] = test_populate_queen_attack;
	test_cases[3] = test_populate_pawn_attack;
	test_cases[4] = test_generate_occupancy_table;
	test_cases[5] = test_square_attacked;
	test_cases[6] = test_is_legal_move;
	test_cases[7] = test_is_legal_move_en_passant;
	test_cases[8] = test_is_legal_move_castling;

	test_case_names[0] = "test_populate_rook_attack";
	test_case_names[1] = "test_populate_bishop_attack";
	test_case_names[2] = "test_populate_queen_attack";
	test_case_names[3] = "test_populate_pawn_attack";
	test_case_names[4] = "test_generate_occupancy_table";
	test_case_names[5] = "test_square_attacked";
	test_case_names[6] = "test_is_legal_move";
	test_case_names[7] = "test_is_legal_move_en_passant";
	test_case_names[8] = "test_is_legal_move_castling";

    return run_tests(test_cases, test_case_names, num_tests);
}
