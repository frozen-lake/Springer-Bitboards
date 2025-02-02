#ifndef BOARD_H
#define BOARD_H

typedef struct {
	unsigned long long white;
	unsigned long long black;

	unsigned long long knight;
	unsigned long long bishop;
	unsigned long long pawn;
	unsigned long long rook;
	unsigned long long queen;
	unsigned long long king;
	

	unsigned long long attack_to[64];
	unsigned long long attack_from[64]; 

} Board;

#endif



char position_to_piece(Board* board, int pos);

void initialize_board(Board* board);

void print_board(Board* board);
void print_bitboard(unsigned long long);

int load_fen(Board* board, char* str);



