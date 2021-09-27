#ifndef COMMON_HANGMAN_GAME_H
#define COMMON_HANGMAN_GAME_H

#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*Funcion que da la opcion de poder elegir como
obtener la letra con la cual jugar*/
typedef char (*hangman_letter_callback_t)(void *letter_callback_ctx);

/*Funcion que da la opcion de poder elegir que
hacer con la informacion de la partida actual*/
typedef void (*hangman_data_callback_t)(unsigned char attempts,
                                        uint16_t word_length,
                                        char* partial_word,
                                        void *data_callback_ctx);

typedef struct hangman_game{
    char* word;
    uint16_t word_length;
    unsigned char attempts;
    bool word_was_guessed;
}hangman_game_t;

void hangman_game_init(hangman_game_t* hangman_game,
                        char* word,
                        unsigned char attempts);

void hangman_game_uninit(hangman_game_t* hangman_game);

//Devuelve 1 en caso de victoria, 0 en caso de derrota
int hangman_game_play(hangman_game_t* hangman_game, 
                hangman_letter_callback_t letter_callback,
                void* letter_callback_ctx,
                hangman_data_callback_t data_callback,
                void* data_callback_ctx,
                hangman_data_callback_t endgame_callback,
                void* endgame_callback_ctx);

#endif
