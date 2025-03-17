#include "move_gen.h"
#include "game.h"


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
    /* Captures */
    Board* board = game->board;
    uint64_t pawns = board->pieces[Pawn] & board->pieces[color];

    uint64_t occupancy = board->pieces[White] | board->pieces[Black];




    while(pawns) {
        int src = get_lsb_index(pawns);
        pawns &= pawns - 1;

        int left_capture = color?src+7:src-9;
        int right_capture = color?src+9:src-7;
        int forward = color?src+8:src-8;
        int double_forward = color?src+16:src-16;

        /* Captures */
        if((src % 8 > 0) && (U64_MASK(left_capture) & board->pieces[!color])){
            Move move = encode_move(src, left_capture, game->board);
            move_list_add(move_list, move);
        }
        if((src % 8 < 7) && (U64_MASK(right_capture) & board->pieces[!color])){
            Move move = encode_move(src, right_capture, game->board);
            move_list_add(move_list, move);
        }

        /* Forward moves */
        if(!(U64_MASK(forward) & occupancy)){
            Move move = encode_move(src, forward, game->board);
            move_list_add(move_list, move);

            /* Double forward */
            if(!(U64_MASK(double_forward) & occupancy)){
                Move move = encode_move(src, double_forward, game->board);
                move_list_add(move_list, move);
            }
        }

        /* En passant */
        if(game->en_passant != -1){
            if((color && ((game->en_passant == src+7) || (game->en_passant == src+9)))
                || (!color && ((game->en_passant == src-7) || (game->en_passant == src-9)))){
                    Move move = src | (game->en_passant << 6) | (Pawn << 12) | (Pawn << 15) | (1 << 21);
                    move_list_add(move_list, move);
             }
        }
    }
}
void generate_king_moves(MoveList* move_list, Game* game, int color){
    Board* board = game->board;
    uint64_t king = board->pieces[King] & board->pieces[color];
    generate_moves(king, move_list, game, color);

    uint64_t occupied = board->pieces[White] | board->pieces[Black];
    int king_pos = get_lsb_index(king);

    if(!(occupied & ((king << 1) | (king << 2)))
            && (board->pieces[color] & board->pieces[Rook] & (king<<3))){
        Move move = king_pos | ((king_pos+2) << 6) | (King << 12) | (Kingside << 21);
        move_list_add(move_list, move);
    }

    if(!(occupied & ((king >> 1) | (king >> 2) | (king >> 3)))
            && (board->pieces[color] & board->pieces[Rook] & (king >> 4))){
        Move move = king_pos | ((king_pos-2) << 6) | (King << 12) | (Queenside << 21);
        move_list_add(move_list, move);
    }
}

void generate_all_moves(MoveList* move_list, Game* game, int color){
    move_list_init(move_list);

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
            move_list_add(move_list, move);
        }
    }
}

void filter_legal_moves(MoveList* move_list, Game* game){
    int num_legal_moves = 0;
    for(int i=0;i<move_list->size;i++){
        if(is_legal_move(game, move_list->moves[i])){
            if(num_legal_moves < i){
                Move tmp = move_list->moves[i];
                move_list->moves[i] = move_list->moves[num_legal_moves];
                move_list->moves[num_legal_moves] = tmp;
            }
            
            num_legal_moves += 1;
        }
    }
    move_list->size = num_legal_moves;
}

void generate_legal_moves(Game* game, int color){
    generate_all_moves(&game->legal_moves, game, color);
    filter_legal_moves(&game->legal_moves, game);
}