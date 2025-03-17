#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "board.h"
#include "attack_data.h"
#include "game.h"


/* Create and return an empty Board. */
Board* create_board(){
	Board* board = (Board*) calloc(1, sizeof(Board));
	return board;
}

void destroy_board(Board* board){
	free(board);
}

/* Initialize a Board with the starting position. */
void initialize_board(Board* board){
	board->pieces[Black] = 0xFFFFULL << 48;
	board->pieces[White] = 0xFFFFULL;

	board->pieces[Pawn] = 0x00ff00000000ff00ULL;
	board->pieces[Knight] = 66ULL + (66ULL << 56);
	board->pieces[Bishop] = 36ULL + (36ULL << 56);
	board->pieces[Rook] = 129ULL + (129ULL << 56);
	board->pieces[Queen] = 8ULL + (8ULL << 56);
	board->pieces[King] = 16ULL + (16ULL << 56);

	populate_attack_maps(board);
}



/* Prints the board state. */
void print_board(Board* board){
	for(int i=7;i>=0;i--){
		printf("| %c", position_to_piece_char(board, i*8));
		for(int j=1;j<8;j++){
			printf(" | %c", position_to_piece_char(board, i*8 + j));
		}
		printf(" |\n");	
	}
	printf("=================================\n");
}

/* Removes all pieces from the Board and clears the attack bitboards. */
void empty_board(Board* board){
	board->pieces[Black] = board->pieces[White] = board->pieces[Pawn] = board->pieces[Knight] = board->pieces[Bishop] = board->pieces[Rook] = board->pieces[Queen] = board->pieces[King] = 0;
	for(int i=0;i<64;i++){
		board->attack_from[i] = 0;
	}
}

/* Prints the passed bitboard in an 8x8 format. */
void print_bitboard(uint64_t bb){
	for(int i=7;i>=0;i--){
		printf("| %llu", (bb >> (i*8))&1);
		for(int j=1;j<8;j++){
			printf(" | %llu", (bb>>(i*8 + j))&1);
		}
		printf(" |\n");	
	}
	printf("=================================\n");
	
}
/* Returns the character representing the piece at the given bit index. */
char position_to_piece_char(Board* board, int pos){
	char c = ' ';
	uint64_t mask = U64_MASK(pos);
	if(board->pieces[Pawn] & mask) c = 'p';
	if(board->pieces[Knight] & mask) c = 'n';
	if(board->pieces[Bishop] & mask) c = 'b';
	if(board->pieces[Rook] & mask) c = 'r';
	if(board->pieces[Queen] & mask) c = 'q';
	if(board->pieces[King] & mask) c = 'k';
	if(board->pieces[White] & mask) c = toupper(c);
	return c; // No piece on this square
}

int position_to_piece_number(Board* board, int pos){
	uint64_t mask = U64_MASK(pos);
	if(board->pieces[Pawn] & mask) return Pawn;
	if(board->pieces[Knight] & mask) return Knight;
	if(board->pieces[Bishop] & mask) return Bishop;
	if(board->pieces[Rook] & mask) return Rook;
	if(board->pieces[Queen] & mask) return Queen;
	if(board->pieces[King] & mask) return King;
	return 0; // value for no piece
}

int square_attacked(Board* board, int square, int attacker_color){
    uint64_t candidates = board->pieces[attacker_color];
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];
    int col = square % 8;
    int row = square / 8;

    uint64_t pawns = board->pieces[Pawn] & candidates;
    if (attacker_color && (attack_data.pawn_black[square] & pawns)) return 1;
    if (!attacker_color && (attack_data.pawn_white[square] & pawns)) return 1;

    if((board->pieces[Knight] & candidates) & attack_data.knight[square]) return 1;
    if((board->pieces[King] & candidates) & attack_data.king[square]) return 1;

    /* Horizontal/vertical sliding pieces */
	uint64_t row_attack = occupancy_table_lookup(col, generate_row_occupancy_key(row, occupancy)) << (8*row); // row
	uint64_t col_attack = occupancy_table_lookup(row, generate_col_occupancy_key(col, occupancy));

	col_attack = col_attack * attack_data.ruld[RULD_INDEX(0)]; // Multiply to convert row to col. Data is in rightmost column
    col_attack = swap_uint64(col_attack); // Vertical flip by reversing
    col_attack = (col_attack >> (7 - col)) & attack_data.col[col]; // Shift to appropriate position and isolate the resulting attack column

	if((row_attack | col_attack) & (candidates & (board->pieces[Rook] | board->pieces[Queen]))){ return 1; }

	/* Diagonal sliding pieces */
    uint64_t ruld_attack = occupancy_table_lookup(col, generate_ruld_occupancy_key(square, occupancy));
    ruld_attack = ((ruld_attack * attack_data.col[0]) & attack_data.ruld[RULD_INDEX(square)]); // Convert attack row to ruld diag, isolate bits

    uint64_t lurd_attack = occupancy_table_lookup(col, generate_lurd_occupancy_key(square, occupancy));
    lurd_attack = ((lurd_attack * attack_data.col[0]) & attack_data.lurd[LURD_INDEX(square)]); // Convert attack row to lurd diag, isolate bits

	if((ruld_attack | lurd_attack) & (candidates & (board->pieces[Bishop] | board->pieces[Queen]))){ return 1; }

    return 0;

}

char* piece_to_string(int piece){
	switch(piece){
		case Pawn:
			return "Pawn";
		case Knight:
			return "Knight";
		case Bishop:
			return "Bishop";
		case Rook:
			return "Rook";
		case Queen:
			return "Queen";
		case King:
			return "King";
		default:
			return "None";
	}
}