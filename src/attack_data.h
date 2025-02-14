
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
};


#endif


void compute_knight_attacks(uint64_t* attack_arr);
void compute_bishop_attacks(uint64_t* attack_arr);
void compute_rook_attacks(uint64_t* attack_arr);

void compute_white_pawn_attacks(uint64_t* attack_arr);
void compute_black_pawn_attacks(uint64_t* attack_arr);
void compute_king_attacks(uint64_t* attack_arr);

AttackData* create_attack_data();
void destroy_attack_data(AttackData*);