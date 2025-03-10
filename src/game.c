#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "game.h"
#include "board.h"
#include "move.h"
#include "attack_data.h"


/* Create, initialize and return a Game. Also creates a Board and sets up the AttackData for this game. */
Game* create_game(){
	Game* game = calloc(1, sizeof(Game));
	game->board = create_board();
	game->side_to_move = 1;
	game->en_passant = 0;

	game->game_length = 0;
	move_list_init(&game->legal_moves);
	game->move_history_capacity = MAX_MOVES;
	game->move_history = (Move*) calloc(game->move_history_capacity, 4);
	return game;
}

/* Destroy a Game. */
void destroy_game(Game* game){
	if(game->board){
		destroy_board(game->board);
		game->board = NULL;
	}
	free(game);
}

/* Set the Board. */
void initialize_game(Game* game){
	initialize_board(game->board);
	game->side_to_move = 1;
}

/* Load a position from a FEN String (Forsyth-Edwards Notation.) */
int load_fen(Game* game, char* str){
	Board* board = create_board();
	// Split up FEN by spaces
	char fen[70];
	strcpy(fen, str);
	char* delim = " ";

	char* fen_field[6]; // pieces, turn, optional castling rights, en passant,  
	fen_field[0] = strtok(fen, delim);
	int fields = 0;
	// printf("field[0]: %s\n", fen_field[0]);
	for(int i=1; i<6 && fen_field[i-1]!=NULL;i++){
		fen_field[i] = strtok(NULL, delim);
		fields++;
		// printf("field[%d]: %s\n", i, fen_field[i]);
	}

	if(fen_field[1]){
		game->side_to_move = (fen_field[1][0] == 'b') ? 0 : 1;
	}
	
	// Split field 0 into ranks by '/'	
	strcpy(fen, fen_field[0]);
	char* ranks[8];
	delim = "/";
	ranks[0] = strtok(fen, delim);
        if(ranks[0] == NULL){ free(board); return 0; }	
	for(int i=1;i<8;i++){
		ranks[i] = strtok(NULL, delim);
		if(ranks[i] == NULL){ free(board); return 0; };
	}
	
	// printf("fen: %s\nfen_field[0]: %s\nranks[7]: %s\n", fen, fen_field[0], ranks[7]);
		
	
	// Process ranks
	for(int rank=7;rank>=0;rank--){
		int len = strlen(ranks[7-rank]);
		int file = 0;
		for(int j=0;j<len;j++){
			if(file > 7){ free(board); return 0; } // Additional space or pieces after capacity is reached
			char c = ranks[7 - rank][j];

			if(isdigit(c)){
				int p = (int) (c - '0');
				if(file + p > 8){
					free(board);
					return 0;
				}
				file += p;
			} else if(isalpha(c)){
				uint64_t piece = 1ULL << (rank*8 + file);
				char u = toupper(c);
				switch(u){
					case 'P':
						board->pieces[Pawn] |= piece;
						break;
					case 'N':
						board->pieces[Knight] |= piece;
						break;
					case 'B':
						board->pieces[Bishop] |= piece;
						break;
					case 'R':
						board->pieces[Rook] |= piece;
						break;
					case 'Q':
						board->pieces[Queen] |= piece;
						break;
					case 'K':
						board->pieces[King] |= piece;
						break;
					default:
						free(board);
						return 0;
				}

				if(isupper(c)){
					board->pieces[White] |= piece;
				} else {
					board->pieces[Black] |= piece;
				}
				file++;
			} else {
				free(board);
				return 0;
			}
		}
		
	}	
	
	if(fen_field[3] && fen_field[3][0] != '-'){
		game->en_passant = parse_square(fen_field[3]);
	}

	//printf("fen: %s\nfields: %s, %s, %s, %s, %s, %s\n", str, fen_field[0], fen_field[1], fen_field[2], fen_field[3], fen_field[4], fen_field[5]);
	//print_board(board);
	//printf("field[3]: %s\n", fen_field[3]);
	// printf("ranks: %s, %s, %s, %s, %s, %s, %s, %s\n", ranks[0], ranks[1], ranks[2], ranks[3], ranks[4], ranks[5], ranks[6], ranks[7]);

	destroy_board(game->board);
	populate_attack_from(board);
	game->board = board;

	return 1;
}


void make_move(Game* game, Move move){
	/* Assume move is legal */
	Board* board = game->board;
	int color = game->side_to_move;

	int dest = get_move_dest(move);
	int src = get_move_src(move);
	int piece_type = get_move_piece(move);
	int capture = get_move_capture(move);

	/* Update piece bitboards */
	board->pieces[piece_type] -= U64_MASK(src);
	board->pieces[piece_type] += U64_MASK(dest);

	if(capture != 0){
		if(!get_move_en_passant(move)){
			board->pieces[capture] -= U64_MASK(dest);
		} else {
			if(color){
				board->pieces[capture] -= U64_MASK(dest - 8);
			} else {
				board->pieces[capture] -= U64_MASK(dest + 8);
			}
		}
	}

	/* Update attack_from at src and dest */
	board->attack_from[src] = 0;
	board->attack_from[dest] = 0;
	generate_attacks(board, dest, piece_type, color);

	/* Update color bitboards */
	if(color){
		board->pieces[White] += U64_MASK(dest);
		board->pieces[White] -= U64_MASK(src);
		board->pieces[Black] &= (-1 - U64_MASK(dest));
	} else {
		board->pieces[Black] += U64_MASK(dest);
		board->pieces[Black] -= U64_MASK(src);
		board->pieces[White] &= (-1 - U64_MASK(dest));
	}
	uint64_t occupancy = board->pieces[White] | board->pieces[Black];

	/* Incremental update rays at src and dest */

	uint64_t update_cols = (attack_data.col[COL_INDEX(src)] | attack_data.col[COL_INDEX(dest)]) & (board->pieces[Rook] | board->pieces[Queen]);
	uint64_t update_rows = (attack_data.row[ROW_INDEX(src)] | attack_data.row[ROW_INDEX(dest)]) & (board->pieces[Rook] | board->pieces[Queen]);

	while(update_cols){
		int update_src = get_lsb_index(update_cols);
		update_cols &= update_cols - 1;
		populate_col_attack(board, update_src, occupancy);
	}
	while(update_rows){
		int update_src = get_lsb_index(update_rows);
		update_rows &= update_rows - 1;
		populate_row_attack(board, update_src, occupancy);
	}


	uint64_t update_ruld = (attack_data.ruld[RULD_INDEX(src)] | attack_data.ruld[RULD_INDEX(dest)]) & (board->pieces[Bishop] | board->pieces[Queen]);
	uint64_t update_lurd = (attack_data.lurd[LURD_INDEX(src)] | attack_data.lurd[LURD_INDEX(dest)]) & (board->pieces[Bishop] | board->pieces[Queen]);

	while(update_ruld){
		int update_src = get_lsb_index(update_ruld);
		update_ruld &= update_ruld - 1;
		populate_ruld_attack(board, update_src, occupancy);
	}
	while(update_lurd){
		int update_src = get_lsb_index(update_lurd);
		update_lurd &= update_lurd - 1;
		populate_lurd_attack(board, update_src, occupancy);
	}

	/* Track next en passant */
	game->en_passant = -1;
	if(piece_type == Pawn){
		if(color && (dest == src+16)){
			game->en_passant = src+8;
		} else if(!color && (dest == src-16)){
			game->en_passant = src-8;
		}
	}

	/* Update move_history */
	if(game->game_length >= game->move_history_capacity){
		game->move_history_capacity *= 2;
		game->move_history = (Move*) realloc(game->move_history, game->move_history_capacity * 4);
	}
	game->move_history[game->game_length] = move;
	game->game_length += 1;


	game->side_to_move = !(game->side_to_move);
}
void undo_move(Game* game, Move move){

}

uint64_t swap_uint64(uint64_t num){
    return __builtin_bswap64(num);
}

int get_lsb_index(uint64_t num){
    return __builtin_ctzll(num);
}