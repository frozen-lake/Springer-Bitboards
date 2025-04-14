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
        int move = parse_algebraic_move(buffer, game);
        free(buffer);
        return move;
    } else {
        printf("Error: BAD INPUT\n");
    }
    free(buffer);
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

    /* MAIN GAME LOOP */
    char move_buffer[16];
    while(1){
        /* PLAYER TURN */
        print_board(game->board);
        printf("Enter move (%s): ", (game->side_to_move?"White":"Black"));

        memset(move_buffer, 0, 16);
        if(fgets(move_buffer, sizeof(move_buffer), stdin) == NULL){
            printf("Error: BAD INPUT\n");
            continue;
        }

        for(int i=0;i<(int)strlen(move_buffer);i++){
            if(move_buffer[i] == '\n'){ move_buffer[i] = '\0'; }
        }

        Move move = parse_algebraic_move(move_buffer, game);

        if(is_legal_player_move(game, move)){
            printf("\n<--%s PLAYED: %s-->\n", (game->side_to_move?"WHITE":"BLACK"), move_buffer);
            make_move(game, move); 
        } else {
            printf("\n<--ILLEGAL MOVE-->\n");
            continue;
        }


        /* AI TURN */
        
        /*
        move = find_move(game);
        if(is_legal_player_move(game, move)){
            printf("\n<--%s PLAYED: %s-->\n", (game->side_to_move?"WHITE":"BLACK"), move_buffer);
            make_move(game, move); 
        } else {
            fprintf(stderr, "\nEXCEPTION: ILLEGAL AI MOVE MOVE\n");
            destroy_game(game);
            return -1;
        }
        */

    }



    return 0;
}