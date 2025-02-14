#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "board.h"


Board* create_board(){
	Board* board = (Board*) calloc(1, sizeof(Board));
	initialize_board(board);
	return board;
}

void destroy_board(Board* board){
	free(board);
}

void initialize_board(Board* board){
	board->pieces[Black] = 0xFFFFULL << 48;
	board->pieces[White] = 0xFFFFULL;

	board->pieces[Pawn] = 0x00ff00000000ff00ULL;
	board->pieces[Knight] = 66ULL + (66ULL << 56);
	board->pieces[Bishop] = 36ULL + (36ULL << 56);
	board->pieces[Rook] = 129ULL + (129ULL << 56);
	board->pieces[Queen] = 8ULL + (8ULL << 56);
	board->pieces[King] = 16ULL + (16ULL << 56);
}

// Returns the character representing the piece at the given bit index
char position_to_piece(Board* board, int pos){
	char c = ' ';
	uint64_t mask = 1ULL << pos;
	if(board->pieces[Pawn] & mask) c = 'p';
	if(board->pieces[Knight] & mask) c = 'n';
	if(board->pieces[Bishop] & mask) c = 'b';
	if(board->pieces[Rook] & mask) c = 'r';
	if(board->pieces[Queen] & mask) c = 'q';
	if(board->pieces[King] & mask) c = 'k';
	if(board->pieces[White] & mask) c = toupper(c);
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
	board->pieces[Black] = board->pieces[White] = board->pieces[Pawn] = board->pieces[Knight] = board->pieces[Bishop] = board->pieces[Rook] = board->pieces[Queen] = board->pieces[King] = 0;
	for(int i=0;i<64;i++){
		board->attack_to[i] = 0;
	}
}

// Prints the passed bitboard in an 8x8 format
void print_bitboard(uint64_t bb){
	for(int i=7;i>=0;i--){
		printf("| %d", (bb >> (i*8))&1);
		for(int j=1;j<8;j++){
			printf(" | %d", (bb>>(i*8 + j))&1);
		}
		printf(" |\n");	
	}
	
}