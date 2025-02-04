#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "game.h"
#include "board.h"
#include "prompt_move.h"
#include "generate_moves.h"


// ====== prompt_move tests

int test_parse_square(){
	Board* board = create_board();
	int f3 = parse_square("f3");
	
	destroy_board(board);
	return f3 == 21;
}

int test_find_source_square(){
	Board* board = create_board();

	// replace with generate_moves once that routine is functional
	board->attack_to[6] |= (1 << 21);
	board->attack_to[12] |= (1 << 28);

	int Nf3_src = find_source_square(board, 'N', 21, 0, -1);
	int e4_src = find_source_square(board, 'P', 28, 0, -1);
	
	// printf("Nf3src: %d, e4src: %d\n", Nf3_src, e4_src);
	
	destroy_board(board);
	return Nf3_src == 6 && e4_src == 12;
}

int test_prompt_move(){
	
	Board* board = create_board();
	char* move = "Nf3";
	
	// replace with generate_moves once that routine is functional
	board->attack_to[6] |= (1 << 21);
	board->attack_to[12] |= (1 << 28);

	int Nf3 = parse_algebraic_move(move, board);
	int e4 = parse_algebraic_move("e4", board);
	
	printf("Encoded move: %d\n", e4);
	
	destroy_board(board);
	return Nf3 == (6 << 6) + 21 && e4 == (12 << 6) + 28;
}

// ====== board tests

int test_load_fen(){
	Game* game = create_game();
	
	char* fen = "4k3/8/8/1n2p3/4P1Pp/8/8/3BK3 b - g3 0 1";
	int success = load_fen(game, fen);
	Board* board = game->board;
	
	// printf("Board after load_fen:\n");
	// print_board(board);
	
	if(!success){ destroy_game(game); return 0; }
	
	if(!board){ destroy_game(game); return 0; }

	int pawn_match = (board->pawn) == ((1ULL << 28) | (1ULL << 36) | (1ULL << 30) | (1ULL << 31));

	// To-Do: test en passant square and turn


	destroy_game(game);
	return pawn_match;
}

// ====== generate_moves tests

int test_generate_pawn_moves(){
	Board* board = create_board();

	generate_pawn_moves(board);	

	// To-Do: verify moves including promotion, en passant, captures, double pawn move.

	destroy_board(board);
	return 0;
}

int test_generate_moves(){
	Board* board = create_board();
	generate_pawn_moves(board);
	
	// To-Do: verify a variety of piece moves, verify legal move bitboards
	destroy_board(board);
	return 0;
}

// ======

int main(){
	int (* test_cases[6])();
	
	//{test_parse_square, test_find_source_square, test_prompt_move, test_load_fen, test_generate_pawn_moves, test_generate_moves};
	char* test_case_names[6];

	test_cases[0] = test_parse_square;
	test_cases[1] = test_find_source_square;
	test_cases[2] = test_prompt_move;
	test_cases[3] = test_load_fen;
	test_cases[4] = test_generate_pawn_moves;
	test_cases[5] = test_generate_moves;
	
	test_case_names[0] = "test_parse_square";
	test_case_names[1] = "test_find_source_square";
	test_case_names[2] = "test_prompt_move";
	test_case_names[3] = "test_load_fen";
	test_case_names[4] = "test_generate_pawn_moves";
	test_case_names[5] = "test_generate_moves";

	int size = sizeof(test_cases) / sizeof(test_cases[0]);

	for(int i=0;i<size;i++){
		if(!test_cases[i]()){
			fprintf(stderr, "Test FAILED: %s \t<-----------\n", test_case_names[i]);
		} else {
			printf("Test passed: %s\n", test_case_names[i]);
		}
	}

	/*
	if(!test_parse_square()){
		fprintf(stderr, "Test failed: ");
	}
	if(!test_find_source_square()){
		fprintf(stderr, "Test failed: test_find_source_square\n");
	}
	if(!test_prompt_move()){
		fprintf(stderr, "Test failed: test_prompt_move\n");
	}

	if(!test_load_fen()){
		fprintf(stderr, "Test failed: test_load_fen\n");
	}

	if(!test_generate_pawn_moves()){
		fprintf(stderr, "Test failed: test_generate_pawn_moves\n");
	}

	if(!test_generate_moves()){
		fprintf(stderr, "Test failed: test_generate_moves\n");
	}
	*/

	return 0;
}
