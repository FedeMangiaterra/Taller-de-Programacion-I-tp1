#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "common_hangman_game.h"
#include "common_socket.h"
#include "server_hangman.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_MESSAGE_LENGTH 65538

void server_hangman_init(server_hangman_t* server,
                        FILE* words_file,
                        unsigned char attempts,
                        const char *host,
                        const char *service){
    server->words_file = words_file;
    socket_t socket;
    socket_init(&socket);
    socket_bind_and_listen(&socket, host, service);
    server->host_socket = socket;
    server->attempts = attempts;
    server->wins = 0;
    server->loses = 0; 
}

void server_hangman_uninit(server_hangman_t* server){
    fclose(server->words_file);
    socket_uninit(&server->host_socket);
}

char obtain_letter(void* socket){
    socket_t* client = socket;
    char letter[1];
    socket_receive(client, letter, 1);
    return letter[0];
}

void send_hangman_data(unsigned char attempts,
                        uint16_t word_length,
                        char* partial_word,
                        void *socket){
    socket_t* client = socket;
    int message_length = 3 + word_length;
    char data_buffer[MAX_MESSAGE_LENGTH];
    if ((strchr(partial_word, '_') == NULL) || attempts == 0){
        attempts += 128; //Valor del bit mas significativo
    }
    data_buffer[0] = attempts;
    uint16_t length = htons(word_length);
    memcpy(&data_buffer[1],&length,2);
    memcpy(&data_buffer[3], partial_word, word_length);
    socket_send(client, data_buffer, message_length);
}

void print_results(server_hangman_t* server){
    printf("Resumen:\n");
    printf("\tVictorias: %d\n", server->wins);
    printf("\tDerrotas: %d\n", server->loses);
}

void server_hangman_play(server_hangman_t* server){
    char* word = NULL;
    int characters_read;
    size_t max_length = 30;
    characters_read = getline(&word, &max_length, server->words_file);
    while (characters_read != -1){
        socket_t client;
        if (socket_accept(&server->host_socket, &client) == 1) return;
        //Saco el salto de linea para quedarme solo con la palabra
        if (word[characters_read-1] == '\n'){
            word[characters_read-1] = '\0';
        }
        hangman_game_t hangman_game;
        hangman_game_init(&hangman_game, word, server->attempts);
        if (hangman_game_play(&hangman_game,
                        obtain_letter,
                        &client,
                        send_hangman_data,
                        &client,
                        send_hangman_data,
                        &client) == 1){
            server->wins++;
        }else{
            server->loses++;
        }
        hangman_game_uninit(&hangman_game);
        socket_uninit(&client);
        free(word);
        word = NULL;
        characters_read = getline(&word, &max_length, server->words_file);
    }
    free(word);
    print_results(server);
}

