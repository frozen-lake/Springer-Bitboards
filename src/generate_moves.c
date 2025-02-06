#include "board.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t** compute_attack_maps(){
	
	/* Allocate a 6 x 64 array of bitboards */
	uint64_t** attack_maps = (uint64_t**) calloc(6, sizeof(uint64_t*));
	for(int i=0;i<6;i++){
		attack_maps[i] = calloc(64, sizeof(uint64_t));
	}



	return attack_maps;
}

void generate_pawn_moves(Board* board){
	uint64_t occupied = board->white | board->black;
	uint64_t white_pawns = board->white & board->pieces[Pawn];
	uint64_t black_pawns = board->black & board->pieces[Pawn];
	
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
