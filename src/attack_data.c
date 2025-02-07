#include "game.h"
#include <stdlib.h>



uint64_t* compute_knight_attacks(){
    uint64_t* knight_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));

    for(int i=0;i<64;i++){
        if(i%8 > 0 && i/8 < 6) knight_attacks[i] |= U64_MASK(15+i);
        if(i%8 < 7 && i/8 <6) knight_attacks[i] |= U64_MASK(17+i);
        if(i%8 > 1 && i/8 < 7) knight_attacks[i] |= U64_MASK(6+i);
        if(i%8 < 6 && i/8 < 7) knight_attacks[i] |= U64_MASK(10+i);
        if(i%8 > 1 && i/8 > 0) knight_attacks[i] |= U64_MASK(i-10);
        if(i%8 < 6 && i/8 > 0) knight_attacks[i] |= U64_MASK(i-6);
        if(i%8 > 0 && i/8 > 1) knight_attacks[i] |= U64_MASK(i-17);
        if(i%8 < 7 && i/8 > 1) knight_attacks[i] |= U64_MASK(i-15);
    }

    return knight_attacks;
}

uint64_t* compute_bishop_attacks(){
    uint64_t* bishop_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));

    return bishop_attacks;
}

uint64_t* compute_rook_attacks(){
    uint64_t* rook_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));

    return rook_attacks;
}

uint64_t* compute_queen_attacks(){
    uint64_t* queen_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));

    return queen_attacks;
}

// it works for bishops, rooks, queen, knight, king
uint64_t** compute_attack_maps(){
	
	/* Allocate a 6 x 64 array of bitboards */
	uint64_t** attack_maps = (uint64_t**) calloc(6, sizeof(uint64_t*));
	
    attack_maps[Knight] = compute_knight_attacks();
    attack_maps[Bishop] = compute_bishop_attacks();
    attack_maps[Rook] = compute_rook_attacks();


	return attack_maps;
}
