#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "board.h"


Board* create_board(){
	Board* board = (Board*) malloc(sizeof(Board));
	initialize_board(board);
	return board;
}

void destroy_board(Board* board){
	free(board);
}

void initialize_board(Board* board){
	board->black = 0xFFFFULL << 48;
	board->white = 0xFFFFULL;

	board->pawn = 0x00ff00000000ff00ULL;
	board->knight = 66ULL + (66ULL << 56);
	board->bishop = 36ULL + (36ULL << 56);
	board->rook = 129ULL + (129ULL << 56);
	board->queen = 8ULL + (8ULL << 56);
	board->king = 16ULL + (16ULL << 56);

	for(int i=0;i<64;i++){
		board->attack_to[i] = 0;
	}
}

// Returns the character representing the piece at the given bit index
char position_to_piece(Board* board, int pos){
	char c = ' ';
	unsigned long long mask = 1ULL << pos;
	if(board->pawn & mask) c = 'p';
	if(board->knight & mask) c = 'n';
	if(board->bishop & mask) c = 'b';
	if(board->rook & mask) c = 'r';
	if(board->queen & mask) c = 'q';
	if(board->king & mask) c = 'k';
	if(board->white & mask) c = toupper(c);
	return c; // No piece on this square
}

// Prints the board state
void print_board(Board* board){
	for(int i=7;i>=0;i--){
		printf("| %c", position_to_piece(board, i*8));
		for(int j=1;j<8;j++){
			printf(" | %c", position_to_piece(board, i*8 + j));
		}
		printf(" |\n");	
	}
}

void empty_board(Board* board){
	board->black = board->white = board->pawn = board->knight = board->bishop = board->rook = board->queen = board->king = 0;
	for(int i=0;i<64;i++){
		board->attack_to[i] = 0;
	}
	
}

// Prints the passed bitboard in an 8x8 format
void print_bitboard(unsigned long long bb){
	for(int i=7;i>=0;i--){
		printf("| %d", (bb >> (i*8))&1);
		for(int j=1;j<8;j++){
			printf(" | %d", (bb>>(i*8 + j))&1);
		}
		printf(" |\n");	
	}
	
}