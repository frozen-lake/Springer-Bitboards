#include "search.h"
#include "move_gen.h"
#include "board.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static int PIECE_VALUES[8] = {0, 0, 100, 300, 300, 500, 900, 10000};

void initialize_searchtrace(SearchTrace* trace){
    if(trace == NULL){
        return;
    }

    trace->best_move = 0;
    trace->best_score = -INF;
    trace->depth = 0;
    trace->nodes = 0;
    trace->pv_length = 0;
    trace->num_root_moves = 0;

    for(int i = 0; i < MAX_SEARCH_PLY; i++){
        trace->pv_line[i] = 0;
    }

    for(int i = 0; i < MAX_MOVES; i++){
        trace->root_moves[i] = 0;
        trace->root_scores[i] = -INF;
    }
}

void print_searchtrace(Game* game, SearchState* search_state){
    if(!DEBUG_SEARCH || game == NULL || search_state == NULL || search_state->trace == NULL){
        printf("No search trace available.\n");
        return;
    }

    SearchTrace* trace = search_state->trace;
    char algebraic[16];

    printf("<--SEARCH TRACE-->\n");
    printf("depth: %d\n", trace->depth);
    printf("nodes: %d\n", trace->nodes);
    printf("best score: %d\n", trace->best_score);
    printf("best move: ");
    if(trace->best_move != 0 && move_to_algebraic(game, trace->best_move, algebraic, sizeof(algebraic))){
        printf("%s\n", algebraic);
    } else {
        printf("(none)\n");
    }

    if(trace->pv_length > 0){
        printf("pv:");
        for(int i = 0; i < trace->pv_length; i++){
            if(trace->pv_line[i] == 0){
                break;
            }
            if(move_to_algebraic(game, trace->pv_line[i], algebraic, sizeof(algebraic))){
                printf(" %s", algebraic);
            }
        }
        printf("\n");
    }

    printf("<--MOVE SCORES-->\n");
    for(int i = 0; i < trace->num_root_moves; i++){
        if(move_to_algebraic(game, trace->root_moves[i], algebraic, sizeof(algebraic))){
            printf("%s: score=%d\n", algebraic, trace->root_scores[i]);
        } else {
            printf("(unknown): score=%d\n", trace->root_scores[i]);
        }
    }
}

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

    if(DEBUG_SEARCH && search_state->trace != NULL){
        initialize_searchtrace(search_state->trace);
        search_state->trace->depth = depth;
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
    int completed_root_moves = 0;

    for(int i = 0; i < moves.size; i++){
        Move move = moves.moves[i];

        if(DEBUG_SEARCH && search_state->trace != NULL && i < MAX_MOVES){
            search_state->trace->root_moves[i] = move;
            search_state->trace->root_scores[i] = -INF;
        }

        UndoInfo undo;
        make_move_on_state(&game->state, move, &undo);

        int score = -alpha_beta(search_state, game, -INF, INF, depth - 1, 1);

        unmake_move_on_state(&game->state, move, &undo);

        if(DEBUG_SEARCH && search_state->trace != NULL && i < MAX_MOVES){
            search_state->trace->root_scores[i] = score;
        }

        completed_root_moves = i + 1;

        if(score > best){
            best = score;
            best_move = move;

            if(DEBUG_SEARCH && search_state->trace != NULL){
                search_state->trace->best_move = move;
                search_state->trace->best_score = score;
                search_state->trace->pv_line[0] = move;
                search_state->trace->pv_length = 1;
            }
        }

        if(is_search_stop_requested(search_state)){
            break;
        }
    }

    search_state->pv_line[0] = best_move;
    search_state->pv_length = 1;

    if(DEBUG_SEARCH && search_state->trace != NULL){
        search_state->trace->best_move = best_move;
        search_state->trace->best_score = best;
        search_state->trace->nodes = search_state->nodes;
        search_state->trace->pv_line[0] = best_move;
        search_state->trace->pv_length = 1;
        search_state->trace->num_root_moves = completed_root_moves;
    }

    return best_move;
}

int initialize_searchstate(SearchState* search_state, TranspositionTable* tt, int max_depth, int* stop){
    if(search_state == NULL){
        return 0;
    }

    search_state->tt = tt;
    search_state->trace = NULL;
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

    if(DEBUG_SEARCH){
        search_state->trace = (SearchTrace*)calloc(1, sizeof(SearchTrace));
        if(search_state->trace == NULL){
            return 0;
        }
        initialize_searchtrace(search_state->trace);
    }

    return 1;
}

void destroy_searchstate(SearchState* search_state){
    if(search_state == NULL){
        return;
    }

    if(search_state->trace != NULL){
        free(search_state->trace);
        search_state->trace = NULL;
    }
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
