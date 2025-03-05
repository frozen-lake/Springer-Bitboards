#include "game.h"
#include "move.h"
#include "attack_data.h"
#include "springer.h"
#include "move_gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_input(){
    char* buffer = calloc(16, sizeof(char));
    if(fgets(buffer, sizeof(buffer), stdin) == NULL){
        printf("\nError: BAD INPUT");
    }
    return buffer;
}

Move get_player_move(Game* game){
    printf("Enter move (%s): ", (game->side_to_move?"White":"Black"));
    char* buffer = calloc(16, sizeof(char));
    buffer[strcspn(buffer, "\r\n")] = '\0'; // strip newline

    if(fgets(buffer, sizeof(buffer), stdin) != NULL){
        printf("\n");
        return parse_algebraic_move(buffer, game);
    } else {
        printf("Error: BAD INPUT");
    }
    return 0;
}

void print_options(){
    printf("=== GAME OPTIONS ===\n");
    printf("1 - New game\n");
    printf("2 - Play from FEN\n");
}

int main(){
	initialize_attack_data();

    Game* game = create_game();


    print_options();
    printf("Enter choice: ");
    char* input = get_input();
    if(input == NULL){
        printf("Error: FAILED MEMORY ALLOCATION");
        return 1;
    }
    printf("input was %s\n", input);

    
    initialize_game(game);
    switch(input[0]){
        case '1':
            break;
        case '2':
            break;
        default:
            break;
    }

    while(1){

        // make move sometimes fails, switches player turn anyway. fix
        print_board(game->board);
        Move move = get_player_move(game);

        if(DEBUG){ print_move(move); }

        if(is_legal_move(game, move)){
            make_move(game, move); 
        } else {

        }
    }



    return 0;
}