#ifndef ATTACK_DATA_H
#define ATTACK_DATA_H

typedef struct AttackData AttackData;

struct AttackData {
    uint64_t row[8];
    uint64_t col[8];

    uint64_t ruld[15]; // right-up, left-down diagonal / indexed by (row-col)+7
    uint64_t lurd[15]; // left-up, right-down diagonal \ indexed by row+col

    uint64_t knight[64];
    uint64_t king[64];
    uint64_t pawn_white[64];
    uint64_t pawn_black[64];

    uint64_t occupancy_table[8][64];
};

extern AttackData attack_data;


#define LURD_INDEX(pos) ((pos/8) + (pos%8))
#define RULD_INDEX(pos) (((pos/8) - (pos%8)) + 7)

#endif


uint64_t occupancy_table_lookup(int index, int occupancy_key);

/* Set board attack_to and attack_from based on occupancy and attack range */
void populate_pawn_attack(Board* board, int origin, int color);
void populate_knight_attack(Board* board, int origin);
void populate_king_attack(Board* board, int origin);
void populate_bishop_attack(Board* board, int origin);
void populate_rook_attack(Board* board, int origin);
void populate_queen_attack(Board* board, int origin);

void populate_attack_from(Board* board);

void initialize_attack_data();