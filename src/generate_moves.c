#include "board.h"
#include <math.h>
#include <stdio.h>


void generate_pawn_moves(Board* board){
	unsigned long long occupied = board->white | board->black;
	unsigned long long white_pawns = board->white & board->pawn;
	unsigned long long black_pawns = board->black & board->pawn;
	
	// Iterate over pawns by repeatedly popping MSB
	while(white_pawns){
		int msb_index = (int) (log(white_pawns) / log(2));	
		unsigned pawn = (unsigned) (1 << msb_index);

		white_pawns -= pawn;	
		board->attack_to[msb_index] = 0;
		
		if(!(occupied & (pawn << 8))){
			if((msb_index / 8 == 1) && !(occupied & (pawn << 16))){ // double pawn move
				board->attack_to[msb_index] |= (pawn << 16);		
			}

			if(msb_index / 8 == 6){ // promotion
				// To-Do: implement promotion	
			}

			board->attack_to[msb_index] |= (pawn << 8); 
		}
		if(msb_index / 8 == 4){ // en passant
			// To-Do: implement en passant
		}
		// printf("white pawns: %lX, msb: %d\n", white_pawns, msb_index);
	}
	// print_bitboard(board->attack_to[12]);
	
	// To-Do: Implement capture, write tests for each case

}

void generate_moves(Board* board){
	generate_pawn_moves(board);	
}
