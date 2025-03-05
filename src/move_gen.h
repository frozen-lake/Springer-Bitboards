#include "move.h"
#include "board.h"


int is_en_passant(int src, int dest, Game* game);
int is_castling(int src, int dest, Game* game);
int is_legal_move(Game* game, Move move);

void generate_knight_moves(MoveList* move_list, Game* game, int color);
void generate_bishop_moves(MoveList* move_list, Game* game, int color);
void generate_rook_moves(MoveList* move_list, Game* game, int color);
void generate_queen_moves(MoveList* move_list, Game* game, int color);

void generate_pawn_moves(MoveList* move_list, Game* game, int color);
void generate_king_moves(MoveList* move_list, Game* game, int color);

void generate_all_moves(MoveList* move_list, Game* game, int color);
void generate_moves(uint64_t movers, MoveList* move_list, Game* game);