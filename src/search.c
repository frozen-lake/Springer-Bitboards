#include "search.h"
#include "move_gen.h"
#include "board.h"
#include "game.h"
#include <stdio.h>

static int PIECE_VALUES[8] = {0, 0, 100, 300, 300, 500, 900, 10000};

Move search_best_move(Game* game, SearchState* search_state){
    if(game == NULL || search_state == NULL){
        return 0;
    }

    int depth = search_state->root_depth;
    if(depth <= 0){
        depth = search_state->max_depth;
    }
    if(depth <= 0){
        depth = 1;
    }
    if(depth > search_state->max_depth){
        depth = search_state->max_depth;
    }

    return search_root(game, search_state, depth);
}

Move search_root(Game* game, SearchState* search_state, int depth){
    if(game == NULL || search_state == NULL){
        return 0;
    }

    if(depth <= 0){
        depth = 1;
    }
    if(depth > search_state->max_depth){
        depth = search_state->max_depth;
    }

    search_state->root_depth = depth;
    search_state->nodes = 0;
    search_state->pv_length = 0;

    MoveList moves;
    generate_all_moves(&moves, game, game->state.side_to_move);
    filter_legal_moves(&moves, game);
    order_moves(&moves, game);

    if(moves.size == 0){
        return 0;
    }

    Move best_move = moves.moves[0];
    int best = -INF;

    for(int i = 0; i < moves.size; i++){
        Move move = moves.moves[i];
        UndoInfo undo;
        make_move_on_state(&game->state, move, &undo);

        int score = -alpha_beta(search_state, game, -INF, INF, depth - 1, 1);

        unmake_move_on_state(&game->state, move, &undo);

        if(score > best){
            best = score;
            best_move = move;
        }

        if(is_search_stop_requested(search_state)){
            break;
        }
    }

    search_state->pv_line[0] = best_move;
    search_state->pv_length = 1;
    return best_move;
}

int initialize_searchstate(SearchState* search_state, TranspositionTable* tt, int max_depth, int* stop){
    if(search_state == NULL){
        return 0;
    }

    search_state->tt = tt;
    search_state->nodes = 0;
    search_state->root_depth = 0;
    search_state->stop = stop;
    search_state->pv_length = 0;

    if(max_depth <= 0){
        search_state->max_depth = MAX_SEARCH_PLY;
    } else if(max_depth > MAX_SEARCH_PLY){
        search_state->max_depth = MAX_SEARCH_PLY;
    } else {
        search_state->max_depth = max_depth;
    }

    return 1;
}

void reset_searchstate(SearchState* search_state){
    if(search_state == NULL){
        return;
    }
    search_state->nodes = 0;
    search_state->root_depth = 0;
    search_state->pv_length = 0;
}

void request_search_stop(SearchState* search_state){
    if(search_state == NULL || search_state->stop == NULL){
        return;
    }
    *(search_state->stop) = 1;
}

void clear_search_stop(SearchState* search_state){
    if(search_state == NULL || search_state->stop == NULL){
        return;
    }
    *(search_state->stop) = 0;
}

int is_search_stop_requested(SearchState* search_state){
    if(search_state == NULL || search_state->stop == NULL){
        return 0;
    }
    return *(search_state->stop) != 0;
}

int evaluate(Game* game){
    Board* board = &game->state;
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

    return game->state.side_to_move == White ? score : -score;
}

int quiesce(SearchState* search_state, Game* game, int alpha, int beta){
    return evaluate(game);
}

int alpha_beta(SearchState* search_state, Game* game, int alpha, int beta, int depth_remaining, int ply){
    if(search_state != NULL){
        search_state->nodes += 1;
    }

    if(is_search_stop_requested(search_state)){
        return evaluate(game);
    }

    if(depth_remaining == 0){
        return quiesce(search_state, game, alpha, beta);
    }

    MoveList moves;
    generate_all_moves(&moves, game, game->state.side_to_move);
    filter_legal_moves(&moves, game);
    order_moves(&moves, game);

    if(moves.size == 0){
        int in_check = square_attacked(&game->state, game->state.king_sq[game->state.side_to_move], !game->state.side_to_move);
        return in_check ? (-MATE_SCORE + ply) : 0;
    }

    int best = -INF;

    for(int i=0; i<moves.size; i++){
        Move move = moves.moves[i];
        UndoInfo undo;
        make_move_on_state(&game->state, move, &undo);

        int score = -alpha_beta(search_state, game, -beta, -alpha, depth_remaining - 1, ply + 1);

        unmake_move_on_state(&game->state, move, &undo);

        if(score > best) best = score;
        if(score > alpha) alpha = score;
        if(alpha >= beta) break; // prune
        if(is_search_stop_requested(search_state)) break;
    }

    return best;
}
