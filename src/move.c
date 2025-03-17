#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "game.h"
#include "move.h"
#include "board.h"

int get_move_src(Move move){ return move & 0b111111; }
int get_move_dest(Move move){ return (move >> 6) & 0b111111; }
int get_move_piece(Move move){ return (move >> 12) & 0b111; }
int get_move_capture(Move move){ return (move >> 15) & 0b111; }
int get_move_promotion(Move move){ return (move >> 18) & 0b111; }
int get_move_special(Move move){ return (move >> 21) & 0b11; }

int is_legal_player_move(Game* game, Move move){
    for(int i=0; i<game->legal_moves.size; i++){
        if(move == game->legal_moves.moves[i]){
            return 1;
        }
    }

    return 0;
}


int is_legal_move(Game* game, Move move){
    if(move == (uint32_t)-1) return 0;

    int color = game->side_to_move;
    int src = get_move_src(move);
    int dest = get_move_dest(move);
    int piece = get_move_piece(move);
    int capture = get_move_capture(move);
    int special = get_move_special(move);
    Board* board = game->board;

    /* Make move:
     * Update piece bitboards
     */
    board->pieces[color] -= U64_MASK(src);
    board->pieces[piece] -= U64_MASK(src);
    if(capture){
        board->pieces[!color] -= U64_MASK(dest);
        board->pieces[capture] -= U64_MASK(dest);
    } 
    board->pieces[color] += U64_MASK(dest);
    board->pieces[piece] += U64_MASK(dest);

    int king_location = get_lsb_index(board->pieces[King] & board->pieces[color]);

    int is_legal = !square_attacked(board, king_location, !color);
    if(((special == Kingside) && square_attacked(board, king_location-1, !color))
        || ((special == Queenside) && square_attacked(board, king_location+1, !color))){
            is_legal = 0;
    }

    /* Undo move:
     * Undo piece bitboard updates
     */
    board->pieces[piece] -= U64_MASK(dest);
    board->pieces[color] -= U64_MASK(dest);
    if(capture){
        board->pieces[capture] += U64_MASK(dest);
        board->pieces[!color] += U64_MASK(dest);
    }
    board->pieces[piece] += U64_MASK(src);
    board->pieces[color] += U64_MASK(src);

    
    return is_legal;
}

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
    uint64_t occupancy = board->pieces[White] | board->pieces[Black];
    for (int i=0; i<64; i++) {
        if (position_to_piece_char(board, i) == piece) { 
            if(piece == 'P'){
                if(((i+8 == destination) || ((i+16 == destination) && i/8 == 1)) && !(occupancy & U64_MASK(destination))){ return i; }
            } else if(piece == 'p'){
                if(((i-8 == destination) || ((i-16 == destination) && i/8 == 6)) && !(occupancy & U64_MASK(destination))){ return i; }
            } else if(board->attack_from[i] & U64_MASK(destination)) {
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
int parse_algebraic_move(char* input, Game* game) {
    Board* board = game->board;
    char piece = 'P'; // Default to pawn
    char file_hint = '\0';
    int rank_hint = -1;
    char destination_square[3] = {0};

    if(input[0] == 'O'){
        uint64_t king = board->pieces[King] & game->board->pieces[game->side_to_move];
        int king_pos = get_lsb_index(king);
        if(strncmp(input, "O-O-O", 5) == 0){
            return king_pos | ((king_pos-2) << 6) | (King << 12) | (Queenside << 21);
        } else if(strncmp(input, "O-O", 3) == 0){
            return king_pos | ((king_pos+2) << 6) | (King << 12) | (Kingside << 21);
        }
    }

    /* Determine the piece and destination square */
    int i = 0;
    if (isalpha(input[0]) && strchr("PNBRQK", input[0])) {
        piece = input[0];
        i++;
    }

    if(!(game->side_to_move)){ piece = tolower(piece); }

    /* Check for disambiguation */
    if(input[1] != 'x'){
        if (isalpha(input[i]) && input[i + 1] >= '1' && input[i + 1] <= '8') { // No disambiguation
            strncpy(destination_square, &input[i], 2);
        } else if (isalpha(input[i]) && isalpha(input[i + 1])) { // File disambiguation
            file_hint = input[i];
            strncpy(destination_square, &input[i + 1], 2);
        } else if (isdigit(input[i]) && isalpha(input[i + 1])) { // Rank disambiguation
            rank_hint = input[i] - '1';
            strncpy(destination_square, &input[i + 1], 2);
        }
    } else {
        strncpy(destination_square, &input[2], 2); // capture dest
    }

    /* Parse destination square */
    int destination = parse_square(destination_square);
    if (DEBUG && destination < 0) {
        fprintf(stderr, "Invalid destination square: %s\n", destination_square);
        return -1;
    }

    /* Check for pawn capture */
    if(strlen(input) == 5 && strchr("abcdefgh", input[0]) && (input[1]=='x')){
        int src = input[0] - 97; // Column
        if(game->side_to_move){ // Row
            src += 8 * (input[3] - 50);
        } else {
            src += 8 * (input[3] - 48);
        }

        if(game->en_passant != destination){
            return encode_move(src, destination, board);
        } else {
            return src | (destination << 6) | (Pawn << 12) | (Pawn << 15) | (1 << 21);
        }
    }


    /* Find the source square */
    int src = find_source_square(board, piece, destination, file_hint, rank_hint);
    if (DEBUG && src < 0) {
	fprintf(stderr, "No valid source square found for move: %spiece: %c, dest: %d, fhint: %d, rhint: %d, source: %d\n",
        input, piece, destination, file_hint, rank_hint, src);
        return -1;
    }

    return encode_move(src, destination, board); // Return the encoded move
}

void print_move(Move move){
    int src = move & 0b111111;
    int dest = (move >> 6) & 0b111111;
    int piece = (move >> 12) & 0b111;
    int capture = (move >> 15) & 0b111;
    int promotion = (move >> 18) & 0b111;
    int special = (move >> 21) & 0b11;

    printf("{move: 0x%x, src: %d, dest: %d, piece: %s, capture: %s, special: %d, promotion: %d}\n",
        move, src, dest, piece_to_string(piece), piece_to_string(capture), special, promotion);
}

void move_list_init(MoveList* move_list){  
    move_list->size = 0;
}

void move_list_add(MoveList* move_list, Move move){
    if(move_list->size < MAX_MOVES){
        move_list->moves[move_list->size] = move;
        move_list->size += 1;
    }
}

void move_list_copy(MoveList* src, MoveList* dest){
    move_list_init(dest);
    for(int i=0;i<src->size;i++){
        move_list_add(dest, src->moves[i]);
    }
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

    int piece = position_to_piece_number(board, src);
    int capture = position_to_piece_number(board, dest);

    move |= piece << 12;
    move |= capture << 15;

    /* Promotion bits and special bits left for later */
    
    return move;
}

Move encode_promotion(int src, int dest, Board* board, int promotion){
    return encode_move(src, dest, board) | (promotion << 18);
}


