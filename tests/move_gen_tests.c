#include <stdio.h>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/game.h"
#include "tests.h"



int test_encode_move(){
	Game* game = create_game();

	load_fen(game, "4k3/8/8/2Q5/4P1Pp/8/8/3BK3 b - g3 0 1");
    Move move = encode_move(34, 2, game->board);
    int success = move == 0x340a2;

    destroy_game(game);
    return success;
}

int test_encode_move_capture(){
	Game* game = create_game();

	load_fen(game, "4k3/8/8/2R5/4P1Pp/8/8/2nBK3 b - g3 0 1");
    Move move = encode_move(34, 2, game->board);
    int success = move == 0xb0a2;

    destroy_game(game);
    return success;
}

int test_encode_move_init(){
    Game* game = create_game();
    initialize_game(game);

    Move Nf3 = encode_move(6, 21, game->board);
    Move e4 = encode_move(12, 28, game->board);

    int success = Nf3 == 0x31546;
    success = success && e4 == 0x3070c;
    return success;
}

void move_gen_tests(){
    int num_tests = 3;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_encode_move;
	test_cases[1] = test_encode_move_capture;
    test_cases[2] = test_encode_move_init;

	test_case_names[0] = "test_encode_move";
	test_case_names[1] = "test_encode_move_capture";
	test_case_names[2] = "test_encode_move_init";

    run_tests(test_cases, test_case_names, num_tests);
}