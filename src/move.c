#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "game.h"

/* Convert algebraic notation to numeric square index: "e4" --> 28 */
int parse_square(const char *square) {
    if (strlen(square) != 2 || square[0] < 'a' || square[0] > 'h' || square[1] < '1' || square[1] > '8') {
        return -1;
    }
    int file = square[0] - 'a';
    int rank = square[1] - '1';
    return rank * 8 + file;
}

/* Find the source square given the piece type and destination square */
int find_source_square(Board *board, char piece, int destination, char file_hint, int rank_hint) {
    for (int i=0; i<64; i++) {
        if (position_to_piece(board, i) == piece) { 
            if(board->attack_to[i] & (1 << destination)) {
                if (file_hint && (i%8) != (file_hint-'a')) {
                    continue;
                }
		        if (rank_hint >= 0 && (i/8) != rank_hint) {
                    continue;
                }
                return i; // Valid source square
            }
        }
    }
    return -1; // No valid source found
}

/* Convert algebraic notation move to integer encoded source/dest squares */
int parse_algebraic_move(char* input, Board *board) {
    char piece = 'P'; // Default to pawn
    char file_hint = '\0';
    int rank_hint = -1;
    char destination_square[3] = {0};

    /* Determine the piece and destination square */
    int i = 0;
    if (isalpha(input[0]) && strchr("PNBRQK", input[0])) {
        piece = input[0];
        i++;
    }

    /* Check for disambiguation */
    if (isalpha(input[i]) && input[i + 1] >= '1' && input[i + 1] <= '8') { // No disambiguation
        strncpy(destination_square, &input[i], 2);
    } else if (isalpha(input[i]) && isalpha(input[i + 1])) { // File disambiguation
        file_hint = input[i];
        strncpy(destination_square, &input[i + 1], 2);
    } else if (isdigit(input[i]) && isalpha(input[i + 1])) { // Rank disambiguation
        rank_hint = input[i] - '1';
        strncpy(destination_square, &input[i + 1], 2);
    }

    /* Parse destination square */
    int destination = parse_square(destination_square);
    if (destination < 0) {
        fprintf(stderr, "Invalid destination square: %s\n", destination_square);
        return -1;
    }

    /* Find the source square */
    int source = find_source_square(board, piece, destination, file_hint, rank_hint);
    if (source < 0) {
	fprintf(stderr, "No valid source square found for move: %s\npiece: %c, dest: %d, fhint: %d, rhint: %d, source: %d\n", input, piece, destination, file_hint, rank_hint, source);
        return -1;
    }

    return (source << 6) + destination; // Return the encoded move
}

/* Populate legal_to and legal_from to reflect legal moves from a rook at bit index origin*/
void generate_rook_moves(Game* game, int origin){
    Board* board = game->board;
    int color = (board->pieces[White] >> origin) & 1;

    uint64_t attack = game->attack_data->rook[origin];
    int col = origin % 8;
    int row = origin / 8;


    /* multiply by special bitboard to */

}

void generate_all_rook_moves(Game* game){

}