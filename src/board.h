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

enum Square {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
};

typedef struct {

	uint64_t attack_from[64]; 
	uint64_t attack_to[64]; // WIP - placeholder, initialized but not maintained via incremental update
	uint64_t pieces[8];


} Board;

#endif



char position_to_piece_char(Board* board, int pos);
int position_to_piece_number(Board* board, int pos);

int square_attacked(Board* board, int square, int attacker_color);

Board* create_board();
void destroy_board(Board* board);

void initialize_board(Board* board);
void empty_board(Board* board);
void print_board(Board* board);

void print_bitboard(uint64_t bb);

char* piece_to_string(int piece);

