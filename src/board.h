#include <stdint.h>

#ifndef BOARD_H
#define BOARD_H

#define U64_MASK(n) (UINT64_C(1) << (n))

enum Piece {
	Black,
	White,
	Pawn,
	Knight,
	Bishop,
	Rook,
	King,
	Queen,
};


typedef struct {

	uint64_t attack_from[64]; 
	uint64_t pieces[8];


} Board;

#endif



char position_to_piece_char(Board* board, int pos);
int position_to_piece_number(Board* board, int pos);

Board* create_board();
void destroy_board(Board* board);

void initialize_board(Board* board);
void empty_board(Board* board);
void print_board(Board* board);

void print_bitboard(uint64_t);

char* piece_to_string(int piece);

