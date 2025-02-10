#include "../src/game.h"
#include "../src/board.h"
#include "../src/attack_data.h"
#include "tests.h"
#include <stdlib.h>
#include <stdio.h>


int test_compute_knight_attacks(){
	uint64_t* knight_attacks = compute_knight_attacks();

	int success = knight_attacks[12] == 0b101000010001000000000001000100;
	success = success && (knight_attacks[0] == U64_MASK(17) | U64_MASK(10));
	success = success && (knight_attacks[63] == U64_MASK(53) | U64_MASK(46));
	success = success && (knight_attacks[7] == U64_MASK(13) | U64_MASK(22));
	success = success && (knight_attacks[57] == U64_MASK(40) | U64_MASK(42) | U64_MASK(51));

	free(knight_attacks);
	return success;
}

int test_compute_bishop_attacks(){
    uint64_t* bishop_attacks = compute_bishop_attacks();

    int success = bishop_attacks[12] == 0b11000001001000100001010000000000000101000;

    free(bishop_attacks);
	return success;
}

int test_compute_rook_attacks(){
    uint64_t* rook_attacks = compute_rook_attacks();

    int success = rook_attacks[12] == 0b1000000010000000100000001000000010000000100001110111100010000;

    free(rook_attacks);
	return success;
}

int test_compute_queen_attacks(){
    uint64_t* queen_attacks = compute_queen_attacks();

    int success = queen_attacks[20] == 0b1000000010001100100100101010000111000111011110011100001010100;

    free(queen_attacks);
	return success;
}


int attack_data_tests(){
    
	int num_tests = 4;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

    test_cases[0] = test_compute_knight_attacks;
	test_cases[1] = test_compute_bishop_attacks;
	test_cases[2] = test_compute_rook_attacks;
	test_cases[3] = test_compute_queen_attacks;

    test_case_names[0] = "test_compute_knight_attacks";
    test_case_names[1] = "test_compute_bishop_attacks";
    test_case_names[2] = "test_compute_rook_attacks";
    test_case_names[3] = "test_compute_queen_attacks";

    run_tests(test_cases, test_case_names, num_tests);

}