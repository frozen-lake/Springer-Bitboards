#include "move_gen.h"
#include "game.h"


void generate_knight_moves(MoveList* move_list, Game* game, int color){
    Board* board = &game->state;
    uint64_t knights = board->pieces[Knight] & board->pieces[color];
    generate_moves(knights, move_list, game, color);
}
void generate_bishop_moves(MoveList* move_list, Game* game, int color){
    Board* board = &game->state;
    uint64_t bishops = board->pieces[Bishop] & board->pieces[color];
    generate_moves(bishops, move_list, game, color);
}
void generate_rook_moves(MoveList* move_list, Game* game, int color){
    Board* board = &game->state;
    uint64_t rooks = board->pieces[Rook] & board->pieces[color];
    generate_moves(rooks, move_list, game, color);
}
void generate_queen_moves(MoveList* move_list, Game* game, int color){
    Board* board = &game->state;
    uint64_t queens = board->pieces[Queen] & board->pieces[color];
    generate_moves(queens, move_list, game, color);
}

void generate_pawn_moves(MoveList* move_list, Game* game, int color){
    /* Captures */
    Board* board = &game->state;
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
            Move move = encode_move(src, left_capture, &game->state);
            move_list_add(move_list, move);
        }
        if((src % 8 < 7) && (U64_MASK(right_capture) & board->pieces[!color])){
            Move move = encode_move(src, right_capture, &game->state);
            move_list_add(move_list, move);
        }

        /* Forward moves */
        if(!(U64_MASK(forward) & occupancy)){
            Move move = encode_move(src, forward, &game->state);
            move_list_add(move_list, move);

            /* Double forward */
            if(!(U64_MASK(double_forward) & occupancy)){
                Move move = encode_move(src, double_forward, &game->state);
                move_list_add(move_list, move);
            }
        }

        /* En passant */
        if(game->state.en_passant != -1){
            if((color && ((game->state.en_passant == src+7) || (game->state.en_passant == src+9)))
                || (!color && ((game->state.en_passant == src-7) || (game->state.en_passant == src-9)))){
                    Move move = src | (game->state.en_passant << 6) | (Pawn << 12) | (Pawn << 15) | (1 << 21);
                    move_list_add(move_list, move);
             }
        }
    }
}
void generate_king_moves(MoveList* move_list, Game* game, int color){
    Board* board = &game->state;
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
    Board* board = &game->state;
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

            Move move = encode_move(src, dest, &game->state);
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

void order_moves(MoveList* move_list, Game* game){
    /* Indexed by victim and by attacker, value is (victim value - attacker value) */
    int MVV_LVA[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, -2, -2, -4, -8, -99},
        {0, 0, 2, 0, 0, -2, -5, -99},
        {0, 0, 2, 0, 0, -2, -5, -99},
        {0, 0, 4, 2, 2, 0, -4, -99},
        {0, 0, 8, 5, 5, 4, 0, -99},
        {0, 0, 99, 99, 99, 99, 99, 0}
    };
    int MVV_LVA_TABLE_OFFSET = 3;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            MVV_LVA[i][j] += MVV_LVA_TABLE_OFFSET;
        }
    }

    int captures_start = 0;
    int captures = 0;

    /* Move captures to front and sort by MVV-LVA */
    for(int i=0;i<move_list->size;i++){
        Move move = move_list->moves[i];
        int piece = get_move_piece(move);
        int capture = get_move_capture(move);
        if(capture){
            Move tmp = move_list->moves[i];
            move_list->moves[i] = move_list->moves[captures];
            move_list->moves[captures] = tmp;

            captures += 1;

            for(int j=captures_start + (captures - 2);j>=captures_start;j--){
                if(MVV_LVA[capture][piece] > MVV_LVA[get_move_capture(move_list->moves[j])][get_move_piece(move_list->moves[j])]){
                    Move tmp = move_list->moves[j];
                    move_list->moves[j] = move_list->moves[j+1];
                    move_list->moves[j+1] = tmp;
                }
            }
        }
    }
}

void generate_legal_moves(Game* game, int color){
    generate_all_moves(&game->legal_moves, game, color);
    filter_legal_moves(&game->legal_moves, game);
    order_moves(&game->legal_moves, game);
}