#include "../src/board.h"
#include "../src/move.h"
#include "../src/game.h"
#include "../src/move_gen.h"
#include "tests.h"

int test_status_active(){
    Game* game = create_game();
    initialize_game(game);
    update_game_status(game);

    int success = (game->game_status == ACTIVE);
    destroy_game(game);
    return success;
}

int test_status_checkmate(){
    Game* game = create_game();

    // White wins with checkmate
    int success = load_fen(game, "7k/6Q1/6K1/8/8/8/8/8 b - - 0 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == WHITE_WINS);
    }

    success = load_fen(game, "2R1k3/8/4K3/8/8/8/8/8 w - - 0 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == WHITE_WINS);
    }

    // Black wins with checkmate
    success = load_fen(game, "8/8/8/8/8/6k1/6q1/7K w - - 0 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == BLACK_WINS);
    }

    destroy_game(game);
    return success;
}

int test_status_checkmate_negative(){
    // The piece putting the king in check can be taken.
    
    Game* game = create_game();
    int success = load_fen(game, "3RK3/3R1p2/4p3/2b2n2/8/6k1/8/3r4 w - - 0 1");

    if(success){
        update_game_status(game);
        success = game->game_status == ACTIVE;
    }

    destroy_game(game);
    return success;
}


int test_status_stalemate(){
    Game* game = create_game();
    int success = load_fen(game, "7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == DRAW_STALEMATE);
    }

    destroy_game(game);
    return success;
}

int test_status_fifty_move_draw(){
    Game* game = create_game();
    int success = load_fen(game, "4k3/8/8/8/8/8/8/4K3 w - - 100 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == DRAW_FIFTY_MOVE);
    }

    destroy_game(game);
    return success;
}

int test_status_insufficient_material_draw(){
    Game* game = create_game();
    int success = load_fen(game, "4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    if(success){
        update_game_status(game);
        success = (game->game_status == DRAW_INSUFFICIENT_MATERIAL);
    }

    destroy_game(game);
    return success;
}

int endgame_tests(){
    int num_tests = 6;

    int (*test_cases[num_tests])();
    char* test_case_names[num_tests];

    test_cases[0] = test_status_active;
    test_cases[1] = test_status_checkmate;
    test_cases[2] = test_status_checkmate_negative;
    test_cases[3] = test_status_stalemate;
    test_cases[4] = test_status_fifty_move_draw;
    test_cases[5] = test_status_insufficient_material_draw;

    test_case_names[0] = "test_status_active";
    test_case_names[1] = "test_status_checkmate";
    test_case_names[2] = "test_status_checkmate_negative";
    test_case_names[3] = "test_status_stalemate";
    test_case_names[4] = "test_status_fifty_move_draw";
    test_case_names[5] = "test_status_insufficient_material_draw";

    return run_tests(test_cases, test_case_names, num_tests);
}