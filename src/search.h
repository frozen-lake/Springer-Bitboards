
Move find_move(Game* game);
int evaluate(Game* game, Board* board);
int alpha_beta(Game*, int alpha, int beta, int depth_remaining);
int quiesce(Game* game, int alpha, int beta);
