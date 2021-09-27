#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "common_hangman_game.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 65535

void hangman_game_init(hangman_game_t* hangman_game,
                        char* word,
                        unsigned char attempts){
    hangman_game->word = word;
    hangman_game->word_length = strlen(word);
    hangman_game->attempts = attempts;
    hangman_game->word_was_guessed = false;
}

void hangman_game_uninit(hangman_game_t* hangman_game){
    //No hace nada
    return;
}

void process_letter(hangman_game_t* hangman_game,
                    char* partial_word,
                    char letter){
    if (strchr(hangman_game->word, letter) == NULL){
            hangman_game->attempts--;
        }else{
            int i;
            for (i = 0; i < hangman_game->word_length; i++){
                if (hangman_game->word[i] == letter) partial_word[i] = letter;
            }
        }
}

int hangman_game_play(hangman_game_t* hangman_game, 
                hangman_letter_callback_t letter_callback,
                void* letter_callback_ctx,
                hangman_data_callback_t data_callback,
                void* data_callback_ctx,
                hangman_data_callback_t endgame_callback,
                void* endgame_callback_ctx){
    int i = 0;
    char partial_word[MAX_WORD_LENGTH]; 
    for (i = 0; i < hangman_game->word_length; i++){
        partial_word[i] = '_';
    }
    partial_word[i] = '\0';
    while (hangman_game->attempts > 0 && !hangman_game->word_was_guessed){
        char letter;
        if (!data_callback){
        /*Si no se le dice que hacer con la informacion, 
        por defecto imprime todo por consola*/  
            printf("Palabra secreta: %s \n", partial_word);
            printf("Te quedan %d intentos \n", hangman_game->attempts);
            printf("Ingrese letra: \n");
        }else{
            data_callback(hangman_game->attempts,
                        hangman_game->word_length,
                        partial_word,
                        data_callback_ctx);
        }
        //Si no se le dice como conseguir la letra, la obtiene por stdin
        if (!letter_callback){ 
            if (scanf(" %c", &letter) != 1){
                printf("Error at reading input with scanf()\n");
                return 0;
            }
        }else{
            letter = letter_callback(letter_callback_ctx);
        }
        process_letter(hangman_game, partial_word, letter);
        if (strcmp(hangman_game->word, partial_word) == 0){
            hangman_game->word_was_guessed = true;
        }
    }
    if (endgame_callback){ 
        endgame_callback(hangman_game->attempts,
                        hangman_game->word_length,
                        hangman_game->word,
                        endgame_callback_ctx);
    }else{  
    /*Si no se le dice que hacer al terminar la partida, 
    imprime el resultado por consola*/   
        if (hangman_game->attempts > 0){
            printf("Ganaste!! \n");
        }else{
            printf("Perdiste! La palabra secreta era: '%s'\n",
                    hangman_game->word);
        }
    }
    if (hangman_game->attempts > 0) return 1;
    return 0;
}
