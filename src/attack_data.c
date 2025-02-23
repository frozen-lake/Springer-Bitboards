#include "game.h"
#include <stdlib.h>

void compute_diagonal_ruld(uint64_t* recipient_arr, int origin){
    /* left, down */
    for(int j = origin; j/8 > 0 && j%8 > 0; j -= 9){
        recipient_arr[origin] |= U64_MASK(j - 9);
    }
    /* right, up */
    for(int j = origin; j/8 < 7 && j%8 < 7; j += 9){
        recipient_arr[origin] |= U64_MASK(j+9);
    }
    recipient_arr[origin] |= U64_MASK(origin);
}
void compute_diagonal_lurd(uint64_t* recipient_arr, int origin){
    /* left, up */
     for(int j = origin; j/8 < 7 && j%8 > 0; j += 7){
        recipient_arr[origin] |= U64_MASK(j+7);
    }
    /* right, down */
    for(int j = origin; j/8 > 0 && j%8 < 7; j -= 7){
        recipient_arr[origin] |= U64_MASK(j-7);
    }
    recipient_arr[origin] |= U64_MASK(origin);
}

void compute_knight_attacks (uint64_t* attack_arr){
    for(int i=0;i<64;i++){
        if(i%8 > 0 && i/8 < 6) attack_arr[i] |= U64_MASK(15+i);
        if(i%8 < 7 && i/8 <6) attack_arr[i] |= U64_MASK(17+i);
        if(i%8 > 1 && i/8 < 7) attack_arr[i] |= U64_MASK(6+i);
        if(i%8 < 6 && i/8 < 7) attack_arr[i] |= U64_MASK(10+i);
        if(i%8 > 1 && i/8 > 0) attack_arr[i] |= U64_MASK(i-10);
        if(i%8 < 6 && i/8 > 0) attack_arr[i] |= U64_MASK(i-6);
        if(i%8 > 0 && i/8 > 1) attack_arr[i] |= U64_MASK(i-17);
        if(i%8 < 7 && i/8 > 1) attack_arr[i] |= U64_MASK(i-15);
    }
}

void compute_bishop_attacks(uint64_t* attack_arr){
    for(int i=0;i<64;i++){
        compute_diagonal_ruld(attack_arr, i);
        compute_diagonal_lurd(attack_arr, i);
    }
}

void compute_rook_attacks(uint64_t* attack_arr){
    for(int i=0;i<64;i++){
        /* left */
        for(int j = i; j%8 > 0; j -= 1){
            attack_arr[i] |= U64_MASK(j - 1);
        }
        /* right */
        for(int j = i; j%8 < 7; j += 1){
            attack_arr[i] |= U64_MASK(j + 1);
        }
        /* down */
        for(int j = i; j/8 > 0; j -= 8){
            attack_arr[i] |= U64_MASK(j - 8);
        }
        /* up */
        for(int j = i; j/8 < 7; j += 8){
            attack_arr[i] |= U64_MASK(j + 8);
        }
    }
}

void compute_queen_attacks(uint64_t* attack_arr){
    for(int i=0;i<64;i++){
        /* left */
        for(int j = i; j%8 > 0; j -= 1){
            attack_arr[i] |= U64_MASK(j - 1);
        }
        /* right */
        for(int j = i; j%8 < 7; j += 1){
            attack_arr[i] |= U64_MASK(j + 1);
        }
        /* down */
        for(int j = i; j/8 > 0; j -= 8){
            attack_arr[i] |= U64_MASK(j - 8);
        }
        /* up */
        for(int j = i; j/8 < 7; j += 8){
            attack_arr[i] |= U64_MASK(j + 8);
        }
    }

    for(int i=0;i<64;i++){
        compute_diagonal_ruld(attack_arr, i);
        compute_diagonal_lurd(attack_arr, i);
    }
}

/* TO-DO: implement pawn, king attack computes */
void compute_white_pawn_attacks(uint64_t* attack_arr){}
void compute_black_pawn_attacks(uint64_t* attack_arr){}
void compute_king_attacks(uint64_t* attack_arr){}

/* Populate legal_to and legal_from to reflect legal moves from a rook at bit index origin*/
void populate_rook_attack(Board* board, AttackData* attack_data, int origin){
    int color = (board->pieces[White] >> origin) & 1;

    uint64_t attack = attack_data->rook[origin];
    int col = origin % 8;
    int row = origin / 8;

    /* Row attack */
    uint64_t blockers = (board->pieces[White] | board->pieces[Black]) & attack_data->rook[origin];





    blockers &= ~board->pieces[color ? Black : White];

}

void populate_bishop_attack(Board* board, AttackData* attack_data, int origin){

}

void populate_queen_attack(Board* board, AttackData* attack_data, int origin){

}

void populate_all_rook_attacks(Board* board, AttackData* attack_data){

}
void populate_all_bishop_attacks(Board* board, AttackData* attack_data){

}

void populate_all_queen_attacks(Board* board, AttackData* attack_data){

}

void generate_occupancy_table(AttackData* attack_data){
    for(int i=0;i<8;i++){
        for(int j=0;j<64;j++){
            unsigned char result = 0;

            int l = i;
            while(l > 0){
                l--;
                result |= (1 << l);
                if((1 << l) & (j << 1)) break;
            }

            l = i;
            while(l < 7){
                l++;
                result |= (1 << l);
                if((1 << l) & (j << 1)) break;
            }

            attack_data->occupancy_table[i][j] = result;
        }
    }
}

AttackData* create_attack_data(){
    AttackData* attack_data = (AttackData*) calloc(1, sizeof(AttackData));
    compute_white_pawn_attacks(attack_data->white_pawn);
    compute_black_pawn_attacks(attack_data->black_pawn);
    compute_knight_attacks(attack_data->knight);
    compute_bishop_attacks(attack_data->bishop);
    compute_rook_attacks(attack_data->rook);
    compute_queen_attacks(attack_data->queen);

    attack_data->row[0] = 0b11111111;
    for(int i=1;i<8;i++){
        attack_data->row[i] = attack_data->row[0] << 0;
    }
    attack_data->col[0] = 0b100000001000000010000000100000001000000010000000100000001;
    for(int i=1; i<8; i++){
        attack_data->col[i] = attack_data->col[0] << i;
    }

    /* RULD */
    for(int i=0;i<64;i++){
        compute_diagonal_ruld(attack_data->ruld, i);
    }

    /* LURD */
    for(int i=0;i<64;i++){
        compute_diagonal_lurd(attack_data->lurd, i);
    }

    generate_occupancy_table(attack_data);

    return attack_data;
}

void destroy_attack_data(AttackData* attack_data){
    free(attack_data);
}