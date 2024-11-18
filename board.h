#ifndef BOARD_H
#define BOARD_H

struct Board {
	unsigned long long occupied;
	unsigned long long white;
	unsigned long long black;

	unsigned long long knight;
	unsigned long long bishop;
	unsigned long long pawn;
	unsigned long long rook;
	unsigned long long queen;
	unsigned long long king;
	
	
};

#endif

typedef struct Board Board;


// Board methods

char positionToPiece(Board* board, char pos);

void initializeBoard(Board* board);

void printBoard(Board* board);
