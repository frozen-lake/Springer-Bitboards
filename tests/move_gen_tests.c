#include <stdio.h>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/game.h"
#include "../src/move_gen.h"
#include "tests.h"

int compare_generated_moves(MoveList* expected, MoveList* actual){
    for(int i=0;i<expected->size;i++){
        int found = 0;
        for(int j=0;j<actual->size;j++){
            if(expected->moves[i] == actual->moves[j]){
                found = 1;
            }
        }
        if(!found){
            return 0;
        }
    }
    return expected->size == actual->size;
}

int test_encode_move(){
	Game* game = create_game();

	load_fen(game, "4k3/8/8/2Q5/4P1Pp/8/8/3BK3 b - g3 0 1");
    Move move = encode_move(34, 2, game->board);
    int success = move == (34 | (2 << 6) | (Queen << 12));

    destroy_game(game);
    return success;
}

int test_encode_move_capture(){
	Game* game = create_game();

	load_fen(game, "4k3/8/8/2R5/4P1Pp/8/8/2nBK3 b - g3 0 1");
    Move move = encode_move(34, 2, game->board);
    int success = move == (34 | (2 << 6) | (Rook << 12) | (Knight << 15));

    destroy_game(game);
    return success;
}

int test_encode_move_init(){
    Game* game = create_game();
    initialize_game(game);

    Move Nf3 = encode_move(6, 21, game->board);
    Move e4 = encode_move(12, 28, game->board);

    int success = Nf3 == (6 | (21 << 6) | (Knight << 12));
    success = success && e4 == (12 | (28 << 6) | (Pawn << 12));

    destroy_game(game);
    return success;
}

int test_generate_knight_moves(){
	Game* game = create_game();
	
	char* fen = "5k2/8/8/4p3/4P1Pp/2P1nN2/8/3BK3 b - g3 0 1";
	int success = load_fen(game, fen);
    
    MoveList generated_moves;
    MoveList expected_moves;
    move_list_init(&generated_moves);
    move_list_init(&expected_moves);
    
    move_list_add(&expected_moves, 20 | (3 << 6) | (Knight << 12) | (Bishop << 15)), // Nxd1
    move_list_add(&expected_moves, 20 | (5 << 6) | (Knight << 12)), // Nf1
    move_list_add(&expected_moves, 20 | (10 << 6) | (Knight << 12)), // Nc2
    move_list_add(&expected_moves, 20 | (14 << 6) | (Knight << 12)), // Ng2
    move_list_add(&expected_moves, 20 | (26 << 6) | (Knight << 12)), // Nc4
    move_list_add(&expected_moves, 20 | (30 << 6) | (Knight << 12) | (Pawn << 15)), // Nxg4
    move_list_add(&expected_moves, 20 | (35 << 6) | (Knight << 12)), // Nd5
    move_list_add(&expected_moves, 20 | (37 << 6) | (Knight << 12)), // Nf5

    generate_knight_moves(&generated_moves, game, Black);
    success = success && compare_generated_moves(&expected_moves, &generated_moves);

    move_list_init(&generated_moves);
    move_list_init(&expected_moves);

    move_list_add(&expected_moves, 21 | (6 << 6) | (Knight << 12)); // Ng1
    move_list_add(&expected_moves, 21 | (11 << 6) | (Knight << 12)); // Nd2
    move_list_add(&expected_moves, 21 | (15 << 6) | (Knight << 12)); // Nh2
    move_list_add(&expected_moves, 21 | (27 << 6) | (Knight << 12)); // Nd4
    move_list_add(&expected_moves, 21 | (31 << 6) | (Knight << 12) | (Pawn << 15)); // Nxh4
    move_list_add(&expected_moves, 21 | (36 << 6) | (Knight << 12) | (Pawn << 15)); // Nxe5
    move_list_add(&expected_moves, 21 | (38 << 6) | (Knight << 12)); // Ng5

    generate_knight_moves(&generated_moves, game, White);
    success = success && compare_generated_moves(&expected_moves, &generated_moves);

    destroy_game(game);
    return success;
}

int test_generate_pawn_moves(){
	Game* game = create_game();
	
	char* fen = "4k3/8/8/8/4p3/2n5/3PP3/4K3 w - - 0 1";
	int success = load_fen(game, fen);
    
    MoveList generated_moves;
    MoveList expected_moves;
    move_list_init(&generated_moves);
    move_list_init(&expected_moves);

    move_list_add(&expected_moves, 11 | (19 << 6) | (Pawn << 12)); // d3
    move_list_add(&expected_moves, 11 | (27 << 6) | (Pawn << 12)); // d4
    move_list_add(&expected_moves, 11 | (18 << 6) | (Pawn << 12) | (Knight << 15)); // dxc3
    move_list_add(&expected_moves, 12 | (20 << 6) | (Pawn << 12)); // e3
    
    generate_pawn_moves(&generated_moves, game, 1);
    success = success && compare_generated_moves(&expected_moves, &generated_moves);

    destroy_game(game);
    return success;
}

int test_generate_pawn_moves_en_passant(){
	Game* game = create_game();
	
	char* fen = "4k3/1p6/1P2p3/4Pp2/2N5/8/8/4K3 w - f6 0 1";
	int success = load_fen(game, fen);
    
    MoveList generated_moves;
    MoveList expected_moves;
    move_list_init(&generated_moves);
    move_list_init(&expected_moves);

    move_list_add(&expected_moves, 36 | (45 << 6) | (Pawn << 12) | (Pawn << 15) | (1 << 21));
    generate_pawn_moves(&generated_moves, game, White);

    success = success && compare_generated_moves(&expected_moves, &generated_moves);

    destroy_game(game);
    return success;
}

int test_generate_king_moves(){
    Game* game = create_game();

    char* fen = "4k3/1p6/4p3/5p2/2N5/8/PPP1PPPP/R3K2R";
    int success = load_fen(game, fen);

    MoveList generated_moves;
    MoveList expected_moves;
    move_list_init(&generated_moves);
    move_list_init(&expected_moves);

    move_list_add(&expected_moves, 4 | (3 << 6) | (King << 12));
    move_list_add(&expected_moves, 4 | (5 << 6) | (King << 12));
    move_list_add(&expected_moves, 4 | (11 << 6) | (King << 12));
    move_list_add(&expected_moves, 4 | (2 << 6) | (King << 12) | (Queenside << 21));
    move_list_add(&expected_moves, 4 | (6 << 6) | (King << 12) | (Kingside << 21));

    generate_king_moves(&generated_moves, game, White);
    success = success && compare_generated_moves(&expected_moves, &generated_moves);

    destroy_game(game);
    return success;
}

void move_gen_tests(){
    int num_tests = 7;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

	test_cases[0] = test_encode_move;
	test_cases[1] = test_encode_move_capture;
    test_cases[2] = test_encode_move_init;
    test_cases[3] = test_generate_knight_moves;
    test_cases[4] = test_generate_pawn_moves;
    test_cases[5] = test_generate_pawn_moves_en_passant;
    test_cases[6] = test_generate_king_moves;

	test_case_names[0] = "test_encode_move";
	test_case_names[1] = "test_encode_move_capture";
	test_case_names[2] = "test_encode_move_init";
	test_case_names[3] = "test_generate_knight_moves";
	test_case_names[4] = "test_generate_pawn_moves";
	test_case_names[5] = "test_generate_pawn_moves_en_passant";
	test_case_names[6] = "test_generate_king_moves";

    run_tests(test_cases, test_case_names, num_tests);
}