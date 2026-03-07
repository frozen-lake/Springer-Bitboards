#ifndef SEARCH_H
#define SEARCH_H

#include "transposition_table.h"

#define MAX_SEARCH_PLY 128
#define INF 50000
#define MATE_SCORE 49000

typedef struct SearchState SearchState;

struct SearchState {
    TranspositionTable* tt;
    int nodes;
    int root_depth;
    int max_depth;
    int* stop;
    Move pv_line[MAX_SEARCH_PLY];
    int pv_length;
};

Move search_best_move(Game* game, SearchState* search_state);
Move search_root(Game* game, SearchState* search_state, int depth);
int evaluate(Game* game);
int alpha_beta(SearchState* search_state, Game* game, int alpha, int beta, int depth_remaining, int ply);
int quiesce(SearchState* search_state, Game* game, int alpha, int beta);

int initialize_searchstate(SearchState* search_state, TranspositionTable* tt, int max_depth, int* stop);
void reset_searchstate(SearchState* search_state);
void request_search_stop(SearchState* search_state);
void clear_search_stop(SearchState* search_state);
int is_search_stop_requested(SearchState* search_state);

#endif