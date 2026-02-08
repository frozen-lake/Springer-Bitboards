#include <stdio.h>
#include "tests.h"
#include "../src/game.h"
#include "../src/search.h"


int test_evaluation(){
    Game* game = create_game();

    char* fen = "3rk3/2bp4/8/7N/5p2/5P2/52P/R3K3 w K - 0 2";
    int success = load_fen(game, fen);

    int eval = evaluate(game, game->board);

    success = success && (eval == 0);

    destroy_game(game);
    return success;
}

void search_tests(){
    int num_tests = 1;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

    
	test_cases[0] = test_evaluation;

	test_case_names[0] = "test_evaluation";

    run_tests(test_cases, test_case_names, num_tests);
}