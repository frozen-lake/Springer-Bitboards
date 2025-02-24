#include "game.h"
#include <stdlib.h>

/* This function is currently dependent on a Microsoft build environment. */
uint64_t swap_uint64(uint64_t num){
    return _byteswap_uint64(num);
}

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

uint64_t occupancy_table_lookup(AttackData* attack_data, int index, int occupancy_key){
    return attack_data->occupancy_table[index][occupancy_key];
}

uint64_t generate_row_occupancy_key(AttackData* attack_data, int row, uint64_t occupancy){
    uint64_t result = occupancy & attack_data->row[row]; // Extract occupancy
    return (result >> ((8*row)+1)) & 0b111111; // Isolate the relevant 6 bits
}

uint64_t generate_col_occupancy_key(AttackData* attack_data, int col, uint64_t occupancy){
    uint64_t result = (occupancy & attack_data->col[col]) >> col; // Extract occupancy, move to A file
    return ((result * attack_data->lurd[7]) >> 57) & 0b111111; // Multiply to convert col to row, shift to first rank and isolate the 6 relevant bits
}


/* Populate legal_to and legal_from to reflect legal moves from a rook at bit index origin*/
void populate_rook_attack(Board* board, AttackData* attack_data, int origin){
    board->attack_from[origin] = 0;
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    /* Row attack */
    uint64_t occupancy_key = generate_row_occupancy_key(attack_data, row, occupancy);
    uint64_t attack = occupancy_table_lookup(attack_data, row, occupancy_key) << (8*row); // Perform lookup

    board->attack_from[origin] |= attack;

    /* Column attack */
    occupancy_key = generate_col_occupancy_key(attack_data, col, occupancy);
    attack = occupancy_table_lookup(attack_data, col, occupancy_key); // Perform lookup

    attack = attack * attack_data->ruld[0]; // Multiply to convert row to col. Data is in rightmost column
    attack = swap_uint64(attack); // Vertical flip by reversing
    attack = (attack >> (7 - col)) & attack_data->col[col]; // Shift to appropriate position and isolate the resulting attack column

    board->attack_from[origin] |= attack;
}

void populate_bishop_attack(Board* board, AttackData* attack_data, int origin){
    board->attack_from[origin] = 0;
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    /* RULD attack */

    /* LURD attack */
}

void populate_queen_attack(Board* board, AttackData* attack_data, int origin){
    // populate_rook_attack(board, attack_data, origin);
    // populate_bishop_attack(board, attack_data, origin);
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
            uint64_t result = 0;

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

void compute_static_bitboards(AttackData* attack_data){
    for(int i=0;i<8;i++){ // Row
        attack_data->row[i] = 0b11111111 << (8*i);
    } 
    for(int i=0; i<8; i++){ // Column
        attack_data->col[i] = 0b100000001000000010000000100000001000000010000000100000001 << i;
    }

    for(int i=0;i<64;i++){ // Right-Up Left-Down Diagonal
        compute_diagonal_ruld(attack_data->ruld, i);
    }
    for(int i=0;i<64;i++){ // Left-Up Right-Down Diagonal
        compute_diagonal_lurd(attack_data->lurd, i);
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

    compute_static_bitboards(attack_data);

    generate_occupancy_table(attack_data);

    return attack_data;
}

void destroy_attack_data(AttackData* attack_data){
    free(attack_data);
}