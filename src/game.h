#include "board.h"
#include "attack_data.h"
#include "move.h"

#ifndef GAME_H
#define GAME_H

#define DEBUG_ERR 0
#define DEBUG_PERF 1

typedef struct Game Game;

struct Game {
	MoveList legal_moves;
	Move* move_history;
	uint8_t* castling_rights_history;
	int* en_passant_history;
	uint64_t* zobrist_history;
	Board* board;
	int side_to_move;
	int game_length;
	int move_history_capacity;

	// Legal en passant square for current turn, -1 if none
	int en_passant;

	// Bit 0 = Black/Kingside, bit 1 = Black/Queenside, bit 2 = White/Kingside, bit 3 = White/Queenside
	uint8_t castling_rights;
};


#endif


Game* create_game();
void destroy_game(Game* game);
void initialize_game(Game* game);
int load_fen(Game* game, char* str);

void make_move(Game* game, Move move);
void unmake_move(Game* game, Move move, uint8_t depth);


/* Utilities */
int get_lsb_index(uint64_t num);
uint64_t swap_uint64(uint64_t num);

