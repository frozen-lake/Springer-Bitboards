#include "move_gen.h"
#include "game.h"

int is_en_passant(int src, int dest, Game* game){
    return 0;
}

int is_castling(int src, int dest, Game* game){
    return 0;
}

int is_legal_move(Game* game, Move move){
    int legal = 1;
    legal = legal && move != -1;
    return legal;
}


void generate_knight_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t knights = board->pieces[Knight] & (color ? board->pieces[White] : board->pieces[Black]);
    generate_moves(knights, move_list, game);
}
void generate_bishop_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t bishops = board->pieces[Bishop] & (color ? board->pieces[White] : board->pieces[Black]);
    generate_moves(bishops, move_list, game);
}
void generate_rook_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t rooks = board->pieces[Rook] & (color ? board->pieces[White] : board->pieces[Black]);
    generate_moves(rooks, move_list, game);
}
void generate_queen_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t queens = board->pieces[Queen] & (color ? board->pieces[White] : board->pieces[Black]);
    generate_moves(queens, move_list, game);
}

void generate_pawn_moves(MoveList* move_list, Game* game, int color){
    /* TO-DO: Implement pawn move generation */
}
void generate_king_moves(MoveList* move_list, Game* game, int color){
    /* TO-DO: Implement king move generation */
}

void generate_all_moves(MoveList* move_list, Game* game, int color){
    move_list->size = 0;

    /* Generate from attack_from bitboard */
    generate_knight_moves(move_list, game, color);
    generate_bishop_moves(move_list, game, color);
    generate_rook_moves(move_list, game, color);
    generate_queen_moves(move_list, game, color);

    /* Need to follow special rules */
    generate_pawn_moves(move_list, game, color);
    generate_king_moves(move_list, game, color);

}

/* Generate moves from the attack_from bitboard */
void generate_moves(uint64_t movers, MoveList* move_list, Game* game){
    Board* board = game->board;
    while(movers) {
        int src = get_lsb_index(movers);
        movers &= movers - 1;

        uint64_t attack = board->attack_from[src];
        while(attack){
            int dest = get_lsb_index(attack);
            attack &= attack - 1;

            Move move = encode_move(src, dest, game->board);
            if(is_legal_move(game, move)){
                move_list_add(move_list, move);
            }

        }
    }
}