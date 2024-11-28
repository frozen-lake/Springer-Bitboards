#include <stdio.h>
#include <ctype.h>
#include "board.h"



void initialize_board(Board* board){
	board->black = 0xFFFFULL << 48;
	board->white = 0xFFFFULL;
	board->occupied = board->black + board->white;

	board->pawn = 0x00ff00000000ff00ULL;
	board->knight = 66ULL + (66ULL << 56);
	board->bishop = 36ULL + (36ULL << 56);
	board->rook = 129ULL + (129ULL << 56);
	board->queen = 8ULL + (8ULL << 56);
	board->king = 16ULL + (16ULL << 56);

	for(int i=0;i<64;i++){
		board->legal_attack[i] = 0;
	}
}

char position_to_piece(Board* board, char pos){
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

void print_board(Board* board){
	for(int i=7;i>=0;i--){
		printf("| %c", position_to_piece(board, i*8));
		for(int j=1;j<8;j++){
			printf(" | %c", position_to_piece(board, i*8 + j));
		}
		printf(" |\n");	
	}
}

