#include "move_gen.h"
#include "game.h"

int is_en_passant(int src, int dest, Game* game){
    return 0;
}

int is_castling(int src, int dest, Game* game){
    return 0;
}

/* Contains additional checks for player specified moves */
int is_legal_player_move(Game* game, Move move){
    for(int i=0; i<game->legal_moves.size; i++){
        if(move == game->legal_moves.moves[i]){
            return 1;
        }
    }

    /* Remove when move generation is implemented */
    if(!DEBUG) return 0;
    if(get_move_capture(move) != 0){
        if(game->side_to_move && (game->board->pieces[White] & U64_MASK(get_move_dest(move)))){
            return 0;
        } else if(!(game->side_to_move) && (game->board->pieces[Black] & U64_MASK(get_move_dest(move)))){
            return 0;
        }
    }
    return is_legal_move(game, move);
}

int is_legal_move(Game* game, Move move){
    if(move < 0) return 0;
    return 1;
}


void generate_knight_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t knights = board->pieces[Knight] & board->pieces[color];
    generate_moves(knights, move_list, game, color);
}
void generate_bishop_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t bishops = board->pieces[Bishop] & board->pieces[color];
    generate_moves(bishops, move_list, game, color);
}
void generate_rook_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t rooks = board->pieces[Rook] & board->pieces[color];
    generate_moves(rooks, move_list, game, color);
}
void generate_queen_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t queens = board->pieces[Queen] & board->pieces[color];
    generate_moves(queens, move_list, game, color);
}

void generate_pawn_moves(MoveList* move_list, Game* game, int color){
    /* TO-DO: Implement pawn move generation */
}
void generate_king_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t kings = board->pieces[King] & board->pieces[color];
    generate_moves(kings, move_list, game, color);

    // TO-DO: Add castling moves
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
void generate_moves(uint64_t movers, MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    while(movers) {
        int src = get_lsb_index(movers);
        movers &= movers - 1;

        uint64_t attack = board->attack_from[src];
        while(attack){
            int dest = get_lsb_index(attack);
            attack &= attack - 1;

            if(board->pieces[color] & U64_MASK(dest)){
                continue;
            }

            Move move = encode_move(src, dest, game->board);
            if(is_legal_move(game, move)){
                move_list_add(move_list, move);
            }

        }
    }
}