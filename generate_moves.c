#include "board.h"
#include <math.h>
#include <stdio.h>


void generate_pawn_moves(Board* board){
	// board->legal_attack[i]
	// board->attack_from[i]

	unsigned long long white_pawns = board->white & board->pawn;
	unsigned long long black_pawns = board->black & board->pawn;
	// Iterate over pawns by repeatedly popping MSB
	while(white_pawns){
		int msb_index = (int) (log(white_pawns) / log(2));
		
		// printf("white_pawns: 0x%lX\nmsb_index: %d\n", white_pawns, msb_index);	
		
		unsigned pawn = (unsigned) (1 << msb_index);
		white_pawns -= pawn;
		
		// To-Do: Use individual pawn bitboard to check possible movement squares.
					
	
	}
	


}

void generate_moves(Board* board){
	generate_pawn_moves(board);	
}
