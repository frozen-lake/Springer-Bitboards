#include "game.h"
#include <stdlib.h>
#include <stdio.h>

AttackData attack_data = {};

/* This function is currently dependent on a Microsoft build environment. */
uint64_t swap_uint64(uint64_t num){
    return _byteswap_uint64(num);
}

static void compute_diagonal_ruld(uint64_t* recipient_arr, int origin){
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

static void compute_diagonal_lurd(uint64_t* recipient_arr, int origin){
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

static void compute_knight_attacks(){
    for(int i=0;i<64;i++){
        if(i%8 > 0 && i/8 < 6) attack_data.knight[i] |= U64_MASK(15+i);
        if(i%8 < 7 && i/8 < 6) attack_data.knight[i] |= U64_MASK(17+i);
        if(i%8 > 1 && i/8 < 7) attack_data.knight[i] |= U64_MASK(6+i);
        if(i%8 < 6 && i/8 < 7) attack_data.knight[i] |= U64_MASK(10+i);
        if(i%8 > 1 && i/8 > 0) attack_data.knight[i] |= U64_MASK(i-10);
        if(i%8 < 6 && i/8 > 0) attack_data.knight[i] |= U64_MASK(i-6);
        if(i%8 > 0 && i/8 > 1) attack_data.knight[i] |= U64_MASK(i-17);
        if(i%8 < 7 && i/8 > 1) attack_data.knight[i] |= U64_MASK(i-15);
    }
}

static void compute_king_attacks(){
    for(int i=0;i<64;i++){
        if(i/8 < 7) attack_data.king[i] |= U64_MASK(i+8);
        if(i/8 > 0) attack_data.king[i] |= U64_MASK(i-8);
        if(i%8 > 0) attack_data.king[i] |= U64_MASK(i-1);
        if(i%8 < 0) attack_data.king[i] |= U64_MASK(i+1);
        if(i/8 < 7 && i%8 > 0) attack_data.king[i] |= U64_MASK(i+7);
        if(i/8 < 7 && i%8 < 7) attack_data.king[i] |= U64_MASK(i+9);
        if(i/8 > 0 && i%8 > 0) attack_data.king[i] |= U64_MASK(i-9);
        if(i/8 > 0 && i%8 < 7) attack_data.king[i] |= U64_MASK(i-7); 
    }
}

static void compute_pawn_attacks(){
    for(int i=0;i<64;i++){
        if(i%8 > 0){ // move diagonally left
            if(i/8 < 7) attack_data.pawn_white[i] |= U64_MASK(i+7);
            if(i/8 > 0) attack_data.pawn_black[i] |= U64_MASK(i-9);
        }
        if(i%8 < 7){ // move diagonally right
            if(i/8 < 7) attack_data.pawn_white[i] |= U64_MASK(i+9);
            if(i/8 > 0) attack_data.pawn_black[i] |= U64_MASK(i-7);
        }
    }
}

uint64_t occupancy_table_lookup(int index, int occupancy_key){
    return attack_data.occupancy_table[index][occupancy_key];
}

static uint64_t generate_row_occupancy_key(int row, uint64_t occupancy){
    uint64_t result = occupancy & attack_data.row[row]; // Extract occupancy
    return (result >> ((8*row)+1)) & 0b111111; // Isolate the relevant 6 bits
}

static uint64_t generate_col_occupancy_key(int col, uint64_t occupancy){
    uint64_t result = (occupancy & attack_data.col[col]) >> col; // Extract occupancy, move to A file
    return ((result * attack_data.lurd[7]) >> 57) & 0b111111; // Multiply to convert col to row, shift to first rank and isolate the 6 relevant bits
}

static uint64_t generate_ruld_occupancy_key(int origin, uint64_t occupancy){
	uint64_t occupancy_key = attack_data.ruld[origin] & occupancy;
	occupancy_key = (occupancy_key * attack_data.col[1]) >> 58;
}
static uint64_t generate_lurd_occupancy_key(int origin, uint64_t occupancy){
	uint64_t occupancy_key = attack_data.lurd[origin] & occupancy;
	occupancy_key = (occupancy_key * attack_data.col[1]) >> 58;
}

/* Populate legal_to and legal_from to reflect legal moves from a rook at bit index origin*/
void populate_rook_attack(Board* board, int origin){
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    /* Row attack */
    uint64_t occupancy_key = generate_row_occupancy_key(row, occupancy);
    uint64_t attack = occupancy_table_lookup(col, occupancy_key) << (8*row); // Perform lookup

    board->attack_from[origin] |= attack;

    /* Column attack */
    occupancy_key = generate_col_occupancy_key(col, occupancy);
    attack = occupancy_table_lookup(row, occupancy_key); // Perform lookup

    attack = attack * attack_data.ruld[0]; // Multiply to convert row to col. Data is in rightmost column
    attack = swap_uint64(attack); // Vertical flip by reversing
    attack = (attack >> (7 - col)) & attack_data.col[col]; // Shift to appropriate position and isolate the resulting attack column

    board->attack_from[origin] |= attack;
}

void populate_bishop_attack(Board* board, int origin){
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    /* RULD attack */
    uint64_t occupancy_key = generate_ruld_occupancy_key(origin, occupancy);
    uint64_t attack = occupancy_table_lookup(col, occupancy_key);
    attack = ((attack * attack_data.col[0]) & attack_data.ruld[origin]); // Convert attack row to ruld diag, isolate bits
    
    board->attack_from[origin] |= attack;

    /* LURD attack */
    occupancy_key = generate_lurd_occupancy_key(origin, occupancy);
    attack = occupancy_table_lookup(col, occupancy_key);
    attack = ((attack * attack_data.col[0]) & attack_data.lurd[origin]); // Convert attack row to lurd diag, isolate bits

    board->attack_from[origin] |= attack;
}

void populate_queen_attack(Board* board, int origin){
    populate_rook_attack(board, origin);
    populate_bishop_attack(board, origin);
}

void populate_all_rook_attacks(Board* board){

}
void populate_all_bishop_attacks(Board* board){

}

void populate_all_queen_attacks(Board* board){

}

void generate_occupancy_table(){
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

            attack_data.occupancy_table[i][j] = result;
        }
    }
}


void initialize_attack_data(){
    for(int i=0;i<8;i++){ // Row
        attack_data.row[i] = (uint64_t)0b11111111 << (8*i);
    } 
    for(int i=0; i<8; i++){ // Column
        attack_data.col[i] = (uint64_t)0b100000001000000010000000100000001000000010000000100000001 << i;
    }

    for(int i=0;i<64;i++){ // Right-Up Left-Down Diagonal
        compute_diagonal_ruld(attack_data.ruld, i);
    }
    for(int i=0;i<64;i++){ // Left-Up Right-Down Diagonal
        compute_diagonal_lurd(attack_data.lurd, i);
    }

    compute_king_attacks();
    compute_knight_attacks();
    compute_pawn_attacks();

    generate_occupancy_table();

}