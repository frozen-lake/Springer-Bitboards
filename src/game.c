#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "game.h"
#include "board.h"
#include "move.h"
#include "move_gen.h"
#include "attack_data.h"


/* Create, initialize and return a Game. Also creates a Board and sets up the AttackData for this game. */
Game* create_game(){
	Game* game = calloc(1, sizeof(Game));
	game->state.side_to_move = 1;
	game->state.en_passant = -1;

	game->state.castling_rights = 0b1111;
	game->state.ply = 0;
	game->move_history_capacity = 512;
	game->move_history = (Move*) calloc(game->move_history_capacity, sizeof(Move));

	move_list_init(&game->legal_moves);

	return game;
}

/* Destroy a Game. */
void destroy_game(Game* game){
	if(!game) return;
	free(game->move_history);
	free(game);
}

/* Set the Board. */
void initialize_game(Game* game){
	initialize_board(&game->state);
	game->state.side_to_move = 1;
	game->state.castling_rights = 0b1111;
	game->state.en_passant = -1;
	game->state.ply = 0;
	move_list_init(&game->legal_moves);
	generate_legal_moves(game, game->state.side_to_move);
}

/* Load a position from a FEN String (Forsyth-Edwards Notation.) */
int load_fen(Game* game, char* str){
	Board* board = &game->state;
	empty_board(board);

	// Split up FEN by spaces
	char fen[70];
	if(str == NULL){
		return 0;
	}
	if(snprintf(fen, sizeof(fen), "%s", str) >= (int)sizeof(fen)){
		return 0;
	}
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
		game->state.side_to_move = (fen_field[1][0] == 'b') ? 0 : 1;
	}
	
	// Split field 0 into ranks by '/' 
	if(fen_field[0] == NULL){ return 0; }
	if(snprintf(fen, sizeof(fen), "%s", fen_field[0]) >= (int)sizeof(fen)){
		return 0;
	}
	char* ranks[8];
	delim = "/";
	ranks[0] = strtok(fen, delim);
        if(ranks[0] == NULL){ return 0; }	
	for(int i=1;i<8;i++){
		ranks[i] = strtok(NULL, delim);
		if(ranks[i] == NULL){ return 0; };
	}
	
	// printf("fen: %s\nfen_field[0]: %s\nranks[7]: %s\n", fen, fen_field[0], ranks[7]);
		
	
	// Process ranks
	for(int rank=7;rank>=0;rank--){
		int len = strlen(ranks[7-rank]);
		int file = 0;
		for(int j=0;j<len;j++){
			if(file > 7){ return 0; } // Additional space or pieces after capacity is reached
			char c = ranks[7 - rank][j];

			if(isdigit(c)){
				int p = (int) (c - '0');
				if(file + p > 8){
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
						return 0;
				}

				if(isupper(c)){
					board->pieces[White] |= piece;
				} else {
					board->pieces[Black] |= piece;
				}
				file++;
			} else {
				return 0;
			}
		}
		
	}	
	
	game->state.castling_rights = 0;
	if(fen_field[2] && fen_field[2][0] != '-'){
		int k = 0;
		while(fen_field[2][k] != '\0'){
			switch(fen_field[2][k]){
				case 'Q':
					game->state.castling_rights |= (1 << 3);
					break;
				case 'K':
					game->state.castling_rights |= (1 << 2);
					break;
				case 'q':
					game->state.castling_rights |= (1 << 1);
					break;
				case 'k':
					game->state.castling_rights |= (1 << 0);
					break;
			}
			k++;
		}
	}

	if(fen_field[3] && fen_field[3][0] != '-'){
		game->state.en_passant = parse_square(fen_field[3]);
	}

	game->state.ply = 0;

	populate_attack_maps(board);
	initialize_zobrist(game);

	generate_legal_moves(game, game->state.side_to_move);

	return 1;
}


void make_move(Game* game, Move move){
	/* Assume move is legal */
	Board* board = &game->state;
	int color = game->state.side_to_move;

	int dest = get_move_dest(move);
	int src = get_move_src(move);
	int piece_type = get_move_piece(move);
	int capture = get_move_capture(move);
	int special = get_move_special(move);

	game->state.castling_history[game->state.ply] = game->state.castling_rights;
	game->state.en_passant_history[game->state.ply] = game->state.en_passant;
	game->state.zobrist_history[game->state.ply] = board->zobrist_hash;
	uint8_t old_castling = game->state.castling_rights;
	int old_en_passant = game->state.en_passant;
	
	int capture_square = dest;
	if(capture && special == EnPassant){
		capture_square = color ? dest - 8 : dest + 8;
	}

	int piece_index = piece_type - Pawn;
	board->zobrist_hash ^= zobrist_keys.piece_square[color][piece_index][src];
	board->zobrist_hash ^= zobrist_keys.piece_square[color][piece_index][dest];
	if(capture){
		int capture_index = capture - Pawn;
		board->zobrist_hash ^= zobrist_keys.piece_square[!color][capture_index][capture_square];
	}
	if(special > EnPassant){
		int rook_src = (special == Kingside) ? src + 3 : src - 4;
		int rook_dest = (special == Kingside) ? src + 1 : src - 1;
		int rook_index = Rook - Pawn;
		board->zobrist_hash ^= zobrist_keys.piece_square[color][rook_index][rook_src];
		board->zobrist_hash ^= zobrist_keys.piece_square[color][rook_index][rook_dest];
	}

	/* Update piece bitboards */
	board->pieces[piece_type] -= U64_MASK(src);
	board->pieces[piece_type] += U64_MASK(dest);

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

	if(capture != 0){
		if(!special){
			board->pieces[capture] -= U64_MASK(dest);
			if(capture == Rook){ // Castling rights
				switch(dest){
					case A1:
						game->state.castling_rights &= 0b0111;
						break;
					case H1:
						game->state.castling_rights &= 0b1011; 
						break;
					case A8:
						game->state.castling_rights &= 0b1101;
						break;
					case H8:
						game->state.castling_rights &= 0b1110;
						break;
				}
			}
			board->attack_from[src] = 0;
			board->attack_from[dest] = 0;
			generate_attacks(board, dest, piece_type, color);
		} else if(special == EnPassant){
			board->pieces[capture] -= U64_MASK(capture_square);
			board->pieces[!color] -= U64_MASK(capture_square);
			board->attack_from[src] = 0;
			board->attack_from[capture_square] = 0;
			generate_attacks(board, dest, piece_type, color);
		}
	} else if(special > EnPassant){
		int rook_src = (special == Kingside) ? src + 3 : src - 4;
		int rook_dest = (special == Kingside) ? src + 1 : src - 1;
		
		board->pieces[color] -= U64_MASK(rook_src);
		board->pieces[color] += U64_MASK(rook_dest);
		board->pieces[Rook] -= U64_MASK(rook_src);
		board->pieces[Rook] += U64_MASK(rook_dest);

		board->attack_from[rook_src] = 0;
		board->attack_from[rook_dest] = 0;
		generate_attacks(board, rook_dest, Rook, color);
	} else {
		board->attack_from[src] = 0;
		board->attack_from[dest] = 0;
		generate_attacks(board, dest, piece_type, color);
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
	game->state.en_passant = -1;
	if(piece_type == Pawn){
		if(color && (dest == src+16)){
			game->state.en_passant = src+8;
		} else if(!color && (dest == src-16)){
			game->state.en_passant = src-8;
		}
	}

	/* Update move_history */
	game->move_history[game->state.ply] = move;
	game->state.ply += 1;

	/* Castling rights */
	if(piece_type == King){
		if(src == E1) game->state.castling_rights &= 0b0011;
		if(src == E8) game->state.castling_rights &= 0b1100;
	} else if(piece_type == Rook){
		if(src == A1) game->state.castling_rights &= 0b0111;
		if(src == H1) game->state.castling_rights &= 0b1011;
		if(src == A8) game->state.castling_rights &= 0b1101;
		if(src == H8) game->state.castling_rights &= 0b1110;
	}

	board->zobrist_hash ^= zobrist_keys.castling_rights[old_castling];
	board->zobrist_hash ^= zobrist_keys.castling_rights[game->state.castling_rights];
	if(old_en_passant != -1){
		board->zobrist_hash ^= zobrist_keys.en_passant_file[old_en_passant % 8];
	}
	if(game->state.en_passant != -1){
		board->zobrist_hash ^= zobrist_keys.en_passant_file[game->state.en_passant % 8];
	}
	board->zobrist_hash ^= zobrist_keys.side_to_move;

	game->state.side_to_move = !(game->state.side_to_move);

	move_list_init(&game->legal_moves);
	generate_legal_moves(game, game->state.side_to_move);
	if(DEBUG_ERR && !board_validate(board)){
		fprintf(stderr, "board_validate failed after make_move\n");
	}

}

void unmake_move(Game* game, Move move, uint8_t depth){
	(void)depth;
	if(game->state.ply <= 0){
		return;
	}

	Board* board = &game->state;
	int index = game->state.ply - 1;
	int color = !game->state.side_to_move;

	int dest = get_move_dest(move);
	int src = get_move_src(move);
	int piece_type = get_move_piece(move);
	int capture = get_move_capture(move);
	int special = get_move_special(move);

	int capture_square = dest;
	if(capture && special == EnPassant){
		capture_square = color ? dest - 8 : dest + 8;
	}

	game->state.side_to_move = color;
	game->state.castling_rights = game->state.castling_history[index];
	game->state.en_passant = game->state.en_passant_history[index];
	board->zobrist_hash = game->state.zobrist_history[index];

	/* Restore moved piece */
	board->pieces[piece_type] -= U64_MASK(dest);
	board->pieces[piece_type] += U64_MASK(src);

	/* Restore color for moved piece */
	if(color){
		board->pieces[White] -= U64_MASK(dest);
		board->pieces[White] += U64_MASK(src);
	} else {
		board->pieces[Black] -= U64_MASK(dest);
		board->pieces[Black] += U64_MASK(src);
	}

	/* Restore castling rook if applicable */
	if(special > EnPassant){
		int rook_src = (special == Kingside) ? src + 3 : src - 4;
		int rook_dest = (special == Kingside) ? src + 1 : src - 1;
		board->pieces[Rook] -= U64_MASK(rook_dest);
		board->pieces[Rook] += U64_MASK(rook_src);
		board->pieces[color] -= U64_MASK(rook_dest);
		board->pieces[color] += U64_MASK(rook_src);
	}

	/* Restore captured piece if applicable */
	if(capture){
		board->pieces[capture] += U64_MASK(capture_square);
		if(color){
			board->pieces[Black] += U64_MASK(capture_square);
		} else {
			board->pieces[White] += U64_MASK(capture_square);
		}
	}

	uint64_t occupancy = board->pieces[White] | board->pieces[Black];

	board->attack_from[dest] = 0;
	board->attack_from[src] = 0;
	
	if(capture){
		board->attack_from[capture_square] = 0;
	}

	/* Regenerate attacks for all restored pieces */
	generate_attacks(board, src, piece_type, color);
	
	if(capture){
		int captured_color = !color;
		generate_attacks(board, capture_square, capture, captured_color);
	}
	
	if(special > EnPassant){
		int rook_src = (special == Kingside) ? src + 3 : src - 4;
		int rook_dest = (special == Kingside) ? src + 1 : src - 1;
		board->attack_from[rook_dest] = 0;
		board->attack_from[rook_src] = 0;
		generate_attacks(board, rook_src, Rook, color);
	}

	/* Update all sliding pieces on affected rays/diagonals */
	uint64_t update_cols = (attack_data.col[COL_INDEX(src)] | attack_data.col[COL_INDEX(dest)]) 
	                       & (board->pieces[Rook] | board->pieces[Queen]);
	uint64_t update_rows = (attack_data.row[ROW_INDEX(src)] | attack_data.row[ROW_INDEX(dest)]) 
	                       & (board->pieces[Rook] | board->pieces[Queen]);

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

	uint64_t update_ruld = (attack_data.ruld[RULD_INDEX(src)] | attack_data.ruld[RULD_INDEX(dest)]) 
	                       & (board->pieces[Bishop] | board->pieces[Queen]);
	uint64_t update_lurd = (attack_data.lurd[LURD_INDEX(src)] | attack_data.lurd[LURD_INDEX(dest)]) 
	                       & (board->pieces[Bishop] | board->pieces[Queen]);

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

	move_list_init(&game->legal_moves);
	generate_legal_moves(game, game->state.side_to_move);
	if(DEBUG_ERR && !board_validate(board)){
		fprintf(stderr, "board_validate failed after unmake_move\n");
	}

	game->state.ply -= 1;
}

uint64_t swap_uint64(uint64_t num){
    return __builtin_bswap64(num);
}

int get_lsb_index(uint64_t num){
    return __builtin_ctzll(num);
}