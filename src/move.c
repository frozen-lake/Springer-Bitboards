#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "game.h"
#include "move.h"

/* Convert algebraic notation to numeric square index: "e4" --> 28 */
int parse_square(char *square) {
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
    if (DEBUG && destination < 0) {
        fprintf(stderr, "Invalid destination square: %s\n", destination_square);
        return -1;
    }

    /* Find the source square */
    int source = find_source_square(board, piece, destination, file_hint, rank_hint);
    if (DEBUG && source < 0) {
	fprintf(stderr, "No valid source square found for move: %s\npiece: %c, dest: %d, fhint: %d, rhint: %d, source: %d\n", input, piece, destination, file_hint, rank_hint, source);
        return -1;
    }

    return (source << 6) + destination; // Return the encoded move
}

void print_move(Move move){
    int src = move & 0b111111;
    int dest = (move >> 6) & 0b111111;
    int piece = (move >> 12) & 0b111;
    int capture = (move >> 15) & 0b111;
    int promotion = (move >> 18) & 0b111;
    int special = (move >> 21) & 0b11;

    printf("{move: %x, src: %d, dest: %d, piece: %s, capture: %s, special: %d, promotion: %d}\n",
        move, src, dest, piece_to_string(piece), piece_to_string(capture), special, promotion);
}

void move_list_add(MoveList* move_list, Move move){
    if(move_list->size < MAX_MOVES){
        move_list->moves[move_list->size] = move;
        move_list->size += 1;
    }
}

void move_list_clear(MoveList* move_list){
    move_list->size = 0;
}

void print_moves(MoveList* move_list){
    printf("MoveList (%d moves):\n", move_list->size);
    int size = move_list->size;
    for(int i=0;i<size;i++){
        print_move(move_list->moves[i]);
    }
}


Move encode_move(int src, int dest, Board* board){
    Move move = 0;

    move |= (src & 0b111111);
    move |= (dest & 0b111111) << 6;

    int piece = get_piece_on_square(src, board);
    int capture = get_piece_on_square(dest, board);

    move |= piece << 12;
    move |= capture << 15;

    // promotion bits and special bits left for later
    
    return move;
}

Move encode_promotion(int src, int dest, Board* board, int promotion){
    return encode_move(src, dest, board) | (promotion << 18);
}


