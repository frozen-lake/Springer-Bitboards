#include <stdio.h>
#include <ctype.h>
#include "board.h"


typedef struct Board Board;


void initializeBoard(Board* board){
	board->black = 0xFFFFULL << 48;
	board->white = 0xFFFFULL;
	board->occupied = board->black + board->white;

	board->pawn = 0x00ff00000000ff00ULL;
	board->knight = 66ULL + (66ULL << 56);
	board->bishop = 36ULL + (36ULL << 56);
	board->rook = 129ULL + (129ULL << 56);
	board->queen = 8ULL + (8ULL << 56);
	board->king = 16ULL + (16ULL << 56);


}

char positionToPiece(Board* board, char pos){
	char c = ' ';

	if(!((board->occupied >> pos) & 0x1)) return c;
	if((board->pawn >> pos) & 0x1) c = 'p';
	if((board->knight >> pos) & 0x1) c = 'n';
	if((board->bishop >> pos) & 0x1) c = 'b';
	if((board->rook >> pos) & 0x1) c = 'r';
	if((board->queen >> pos) & 0x1) c = 'q';
	if((board->king >> pos) & 0x1) c = 'k';
	
	if((board->white >> pos) & 0x1) c = toupper(c);

	return c;

}

void printBoard(Board* board){
	for(int i=7;i>=0;i--){
		printf("| %c", positionToPiece(board, i*8));
		for(int j=1;j<8;j++){
			printf(" | %c", positionToPiece(board, i*8 + j));
		}
		printf(" |\n");	
	}
}

