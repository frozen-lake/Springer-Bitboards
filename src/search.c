#include "board.h"
#include "game.h"
#include <stdio.h>

static int PIECE_VALUES[8] = {0, 0, 100, 300, 300, 500, 900, 10000};

Move find_move(Game* game){
    return game->legal_moves.moves[0];
}

int evaluate(Game* game, Board* board){
    int score = 0;
    for(int i=0;i<64;i++){
        uint64_t mask = U64_MASK(i);
        if(board->pieces[White] & mask){
            int piece = position_to_piece_number(board, i);
            score += PIECE_VALUES[piece];
        } else if(board->pieces[Black] & mask){
            int piece = position_to_piece_number(board, i);
            score -= PIECE_VALUES[piece];
        }
    }

    return score;
}

int quiesce(Game* game, int alpha, int beta){
    return -1;
}

int alpha_beta(Game* game, int alpha, int beta, int depth_remaining){
    if(depth_remaining == 0) quiesce(game, alpha, beta);
    for(int i=0;i<game->legal_moves.size;i++){
        
    }
    return -1;
}
