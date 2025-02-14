#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "game.h"
#include "board.h"
#include "attack_data.h"


Game* create_game(){
	Game* game = calloc(1, sizeof(Game));
	initialize_game(game);
	return game;
}

void destroy_game(Game* game){
	if(game->board) destroy_board(game->board);
	if(game->attack_data) destroy_attack_data(game->attack_data);
	free(game);
}

void initialize_game(Game* game){
	game->board = create_board();
    game->attack_data = create_attack_data();

}

int load_fen(Game* game, char* str){
	Board* board = create_board();
	empty_board(board);	
	// Split up FEN by spaces
	char fen[70];
	strcpy(fen, str);
	char* delim = " ";

	char* fen_field[6]; // pieces, turn, optional castling rights, en passant,  
	fen_field[0] = strtok(fen, delim);
	int fields = 0;
	// printf("field[0]: %s\n", fen_field[0]);
	for(int i=1; i<6 && fen_field[i-1]!=NULL;i++){
		fen_field[i] = strtok(NULL, delim);
		fields++;
		// printf("field[%d]: %s\n", i, fen_field[i]);
	}

	
	// Split field 0 into ranks by '/'	
	strcpy(fen, fen_field[0]);
	char* ranks[8];
	delim = "/";
	ranks[0] = strtok(fen, delim);
        if(ranks[0] == NULL){ free(board); return 0; }	
	for(int i=1;i<8;i++){
		ranks[i] = strtok(NULL, delim);
		if(ranks[i] == NULL){ free(board); return 0; };
	}
	
	// printf("fen: %s\nfen_field[0]: %s\nranks[7]: %s\n", fen, fen_field[0], ranks[7]);
		
	
	// Process ranks
	for(int rank=7;rank>=0;rank--){
		int len = strlen(ranks[7-rank]);
		int file = 0;
		for(int j=0;j<len;j++){
			if(file > 7){ free(board); return 0; } // Additional space or pieces after capacity is reached
			char c = ranks[7 - rank][j];

			if(isdigit(c)){
				int p = (int) (c - '0');
				if(file + p > 8){
					free(board); return 0;
				}
				file += p;
			} else if(isalpha(c)){
				uint64_t piece = 1ULL << (rank*8 + file);
				char u = toupper(c);
				switch(u){
					case 'P':
						board->pieces[Pawn] |= piece;
						break;
					case 'N':
						board->pieces[Knight] |= piece;
						break;
					case 'B':
						board->pieces[Bishop] |= piece;
						break;
					case 'R':
						board->pieces[Rook] |= piece;
						break;
					case 'Q':
						board->pieces[Queen] |= piece;
						break;
					case 'K':
						board->pieces[King] |= piece;
						break;
					default:
						free(board);
						return 0;
				}

				if(isupper(c)){
					board->pieces[White] |= piece;
				} else {
					board->pieces[Black] |= piece;
				}
				file++;
			} else {
				free(board);
				return 0;
			}
		}
	}	
	


	// printf("fen: %s\nfields: %s, %s, %s, %s, %s, %s\n", str, fen_field[0], fen_field[1], fen_field[2], fen_field[3], fen_field[4], fen_field[5]);
	
	// printf("field[0]: %s\n", fen_field[0]);
	// printf("ranks: %s, %s, %s, %s, %s, %s, %s, %s\n", ranks[0], ranks[1], ranks[2], ranks[3], ranks[4], ranks[5], ranks[6], ranks[7]);


	game->board = board;
	return 1;
}

