#include "game.h"
#include <stdlib.h>
#include <stdio.h>

AttackData attack_data = {}; // globally declared, statically allocated


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
        if(i%8 < 7) attack_data.king[i] |= U64_MASK(i+1);
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

uint64_t generate_row_occupancy_key(int row, uint64_t occupancy){
    uint64_t result = occupancy & attack_data.row[row]; // Extract occupancy
    return (result >> ((8*row)+1)) & 0b111111; // Isolate the relevant 6 bits
}

uint64_t generate_col_occupancy_key(int col, uint64_t occupancy){
    uint64_t result = (occupancy & attack_data.col[col]) >> col; // Extract occupancy, move to A file
    return ((result * attack_data.lurd[LURD_INDEX(7)]) >> 57) & 0b111111; // Multiply to convert col to row, shift to first rank and isolate the 6 relevant bits
}

uint64_t generate_ruld_occupancy_key(int origin, uint64_t occupancy){
	uint64_t occupancy_key = attack_data.ruld[RULD_INDEX(origin)] & occupancy;
	occupancy_key = (occupancy_key * attack_data.col[1]) >> 58;
    return occupancy_key;
}
uint64_t generate_lurd_occupancy_key(int origin, uint64_t occupancy){
	uint64_t occupancy_key = attack_data.lurd[LURD_INDEX(origin)] & occupancy;
	occupancy_key = (occupancy_key * attack_data.col[1]) >> 58;
    return occupancy_key;
}


void populate_row_attack(Board* board, int origin, uint64_t occupancy){
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy_key = generate_row_occupancy_key(row, occupancy);
    uint64_t attack = occupancy_table_lookup(col, occupancy_key) << (8*row);

    board->attack_from[origin] |= attack;
}
void populate_col_attack(Board* board, int origin, uint64_t occupancy){
    int col = origin % 8;
    int row = origin / 8;
    uint64_t occupancy_key = generate_col_occupancy_key(col, occupancy);
    uint64_t attack = occupancy_table_lookup(row, occupancy_key);

    attack = attack * attack_data.ruld[RULD_INDEX(0)]; // Multiply to convert row to col. Data is in rightmost column
    attack = swap_uint64(attack); // Vertical flip by reversing
    attack = (attack >> (7 - col)) & attack_data.col[col]; // Shift to appropriate position and isolate the resulting attack column

    board->attack_from[origin] |= attack;
}
void populate_ruld_attack(Board* board, int origin, uint64_t occupancy){
    int col = origin % 8;
    uint64_t occupancy_key = generate_ruld_occupancy_key(origin, occupancy);
    uint64_t attack = occupancy_table_lookup(col, occupancy_key);
    attack = ((attack * attack_data.col[0]) & attack_data.ruld[RULD_INDEX(origin)]); // Convert attack row to ruld diag, isolate bits
    
    board->attack_from[origin] |= attack;
}
void populate_lurd_attack(Board* board, int origin, uint64_t occupancy){
    int col = origin % 8;
    uint64_t occupancy_key = generate_lurd_occupancy_key(origin, occupancy);
    uint64_t attack = occupancy_table_lookup(col, occupancy_key);
    attack = ((attack * attack_data.col[0]) & attack_data.lurd[LURD_INDEX(origin)]); // Convert attack row to lurd diag, isolate bits

    board->attack_from[origin] |= attack;
}


/* Populate legal_to and legal_from to reflect legal moves from a Bishop at bit index origin */
void populate_bishop_attack(Board* board, int origin){
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    populate_ruld_attack(board, origin, occupancy);
    populate_lurd_attack(board, origin, occupancy);   
}

/* Populate legal_to and legal_from to reflect legal moves from a rook at bit index origin */
void populate_rook_attack(Board* board, int origin){
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];

    populate_row_attack(board, origin, occupancy);
    populate_col_attack(board, origin, occupancy);
}

void populate_queen_attack(Board* board, int origin){
    populate_rook_attack(board, origin);
    populate_bishop_attack(board, origin);
}

void populate_pawn_attack(Board* board, int origin, int color){
    board->attack_from[origin] = color ? attack_data.pawn_white[origin] : attack_data.pawn_black[origin];
}
void populate_knight_attack(Board* board, int origin){
    board->attack_from[origin] = attack_data.knight[origin];
}
void populate_king_attack(Board* board, int origin){
    board->attack_from[origin] = attack_data.king[origin];
}

void populate_attack_maps(Board* board){
    for(int i=0;i<64;i++){
        board->attack_from[i] = 0;
        board->attack_to[i] = 0;
    }

    uint64_t pieces = board->pieces[Pawn] & board->pieces[White];
    while(pieces){
        populate_pawn_attack(board, get_lsb_index(pieces), 1);
        pieces &= pieces - 1;
    }

    pieces = board->pieces[Pawn] & board->pieces[Black];
    while(pieces){
        populate_pawn_attack(board, get_lsb_index(pieces), 0);
        pieces &= pieces - 1;
    }
    pieces = board->pieces[Knight];
    while(pieces){
        populate_knight_attack(board, get_lsb_index(pieces));
        pieces &= pieces - 1;
    }

    pieces = board->pieces[Bishop];
    while(pieces){
        populate_bishop_attack(board, get_lsb_index(pieces));
        pieces &= pieces - 1;
    }

    pieces = board->pieces[Rook];
    while(pieces){
        populate_rook_attack(board, get_lsb_index(pieces));
        pieces &= pieces - 1;
    }

    pieces = board->pieces[Queen];
    while(pieces){
        populate_queen_attack(board, get_lsb_index(pieces));
        pieces &= pieces - 1;
    }

    pieces = board->pieces[King];
    while(pieces){
        populate_king_attack(board, get_lsb_index(pieces));
        pieces &= pieces - 1;
    }

    /* Iterate over each attack_from attacker, updating the corresponding attack_to square  */
    for(int i=0;i<64;i++){
        uint64_t attacked_squares = board->attack_from[i];
        while(attacked_squares > 0){
            int attacked_square = get_lsb_index(attacked_squares);
            attacked_squares &= attacked_squares -1;
            board->attack_to[attacked_square] |= U64_MASK(i);
        }
    }
}

void generate_attacks(Board* board, int pos, int piece_type, int color){
    switch(piece_type){
		case Pawn:
			populate_pawn_attack(board, pos, color);
			break;
		case Knight:
			populate_knight_attack(board, pos);
			break;
		case Bishop:
			populate_bishop_attack(board, pos);
			break;
		case Rook:
			populate_rook_attack(board, pos);
            break;
        case Queen:
            populate_queen_attack(board, pos);
			break;
		case King:
			populate_king_attack(board, pos);
			break;
		default:
			break;
	}
}

static void generate_occupancy_table(){
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

static void compute_diagonal_ruld(int index) {
    attack_data.ruld[index] = 0;

    for (int col = 0; col < 8; col++) {
        int row = col + (index - 7);
        if (row >= 0 && row < 8) {  // Ensure row is valid
            int pos = row * 8 + col;
            attack_data.ruld[index] |= U64_MASK(pos);
        }
    }
}

static void compute_diagonal_lurd(int index) {
    attack_data.lurd[index] = 0;
    for (int row = 0; row < 8; row++) {
        int col = index - row;
        if (col >= 0 && col < 8) {  // Ensure col is valid
            int pos = row * 8 + col;
            attack_data.lurd[index] |= U64_MASK(pos);
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

    for(int i=0;i<15;i++){ // Right-Up Left-Down Diagonal
        compute_diagonal_ruld(i);
        compute_diagonal_lurd(i);
    }

    compute_king_attacks();
    compute_knight_attacks();
    compute_pawn_attacks();

    generate_occupancy_table();

}