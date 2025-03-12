#include "move.h"
#include "board.h"



void generate_knight_moves(MoveList* move_list, Game* game, int color);
void generate_bishop_moves(MoveList* move_list, Game* game, int color);
void generate_rook_moves(MoveList* move_list, Game* game, int color);
void generate_queen_moves(MoveList* move_list, Game* game, int color);

void generate_pawn_moves(MoveList* move_list, Game* game, int color);
void generate_king_moves(MoveList* move_list, Game* game, int color);

void generate_all_moves(MoveList* move_list, Game* game, int color);
void generate_moves(uint64_t movers, MoveList* move_list, Game* game, int color);

void filter_legal_moves(MoveList* move_list, Game* game);
void generate_legal_moves(Game* game, int color);