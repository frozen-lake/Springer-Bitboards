#include <stdint.h>

#ifndef BOARD_H
#define BOARD_H

enum Piece {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};


typedef struct {
	uint64_t white;
	uint64_t black;

	uint64_t pieces[6];

	uint64_t attack_to[64];
	uint64_t attack_from[64]; 

} Board;

#endif



char position_to_piece(Board* board, int pos);

Board* create_board();
void destroy_board(Board* board);

void initialize_board(Board* board);
void empty_board(Board* board);
void print_board(Board* board);

void print_bitboard(uint64_t);



