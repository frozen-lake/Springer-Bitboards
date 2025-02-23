#ifndef ATTACK_DATA_H
#define ATTACK_DATA_H

typedef struct AttackData AttackData;

struct AttackData {
    uint64_t white_pawn[64];
    uint64_t black_pawn[64];
    uint64_t knight[64];
    uint64_t bishop[64];
    uint64_t rook[64];
    uint64_t queen[64];
    uint64_t king[64];

    uint64_t row[8];
    uint64_t col[8];

    uint64_t ruld[64]; // right-up, left-down diagonal /
    uint64_t lurd[64]; // left-up, right-down diagonal \

    unsigned char occupancy_table[8][64];
};


#endif



/* Compute attack ranges on an otherwise empty bitboard */
void compute_diagonal_ruld(uint64_t* recipient_arr, int origin);
void compute_diagonal_lurd(uint64_t* recipient_arr, int origin);

void compute_knight_attacks(uint64_t* attack_arr);
void compute_bishop_attacks(uint64_t* attack_arr);
void compute_rook_attacks(uint64_t* attack_arr);
void compute_white_pawn_attacks(uint64_t* attack_arr);
void compute_black_pawn_attacks(uint64_t* attack_arr);
void compute_king_attacks(uint64_t* attack_arr);


/* Set board attack_to and attack_from based on occupancy and attack range */
void populate_rook_attack(Board* board, AttackData* attack_data, int origin);
void populate_bishop_attack(Board* board, AttackData* attack_data, int origin);
void populate_queen_attack(Board* board, AttackData* attack_data, int origin);

void populate_all_rook_attacks(Board* board, AttackData* attack_data);
void populate_all_bishop_attacks(Board* board, AttackData* attack_data);
void populate_all_queen_attacks(Board* board, AttackData* attack_data);

AttackData* create_attack_data();
void destroy_attack_data(AttackData* attack_data);