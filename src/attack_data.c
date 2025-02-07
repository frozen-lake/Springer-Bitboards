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

    for(int i=0;i<64;i++){
        /* left, down */
        for(int j = i; j/8 > 0 && j%8 > 0; j -= 9){
            bishop_attacks[i] |= U64_MASK(j - 9);
        }

        /* left, up */
        for(int j = i; j/8 < 7 && j%8 > 0; j += 7){
            bishop_attacks[i] |= U64_MASK(j+7);
        }

        /* right, down */
        for(int j = i; j/8 > 0 && j%8 < 7; j -= 7){
            bishop_attacks[i] |= U64_MASK(j-7);
        }

        /* right, up */
        for(int j = i; j/8 < 7 && j%8 < 7; j += 9){
            bishop_attacks[i] |= U64_MASK(j+9);
        }

    }

    return bishop_attacks;
}

uint64_t* compute_rook_attacks(){
    uint64_t* rook_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));

    for(int i=0;i<64;i++){
        /* left */
        for(int j = i; j%8 > 0; j -= 1){
            rook_attacks[i] |= U64_MASK(j - 1);
        }
        /* right */
        for(int j = i; j%8 < 7; j += 1){
            rook_attacks[i] |= U64_MASK(j + 1);
        }
        /* down */
        for(int j = i; j/8 > 0; j -= 8){
            rook_attacks[i] |= U64_MASK(j - 8);
        }
        /* up */
        for(int j = i; j/8 < 7; j += 8){
            rook_attacks[i] |= U64_MASK(j + 8);
        }
    }


    return rook_attacks;
}

uint64_t* compute_queen_attacks(){
    uint64_t* bishop_attacks = compute_bishop_attacks();
    uint64_t* rook_attacks = compute_rook_attacks();

    uint64_t* queen_attacks = (uint64_t*) calloc(64, sizeof(uint64_t));
    for(int i=0; i<64; i++){
        queen_attacks[i] = bishop_attacks[i] | rook_attacks[i];
    }

    free(bishop_attacks);
    free(rook_attacks);

    return queen_attacks;
}
