#include "common_socket.h"
#include "client_hangman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX_SIZE 30
#define WORD_MAX_LENGTH 65535

void client_hangman_init(client_hangman_t* self,
                         const char *host,
                        const char *service){
    socket_t client;
    socket_init(&client);
    socket_connect(&client, host, service);
    self->client_socket = client;
}

void client_hangman_uninit(client_hangman_t* self){
    socket_uninit(&self->client_socket);
}

void print_info(char* partial_word, unsigned char attempts){
    printf("Palabra secreta: %s \n", partial_word);
    printf("Te quedan %d intentos \n", attempts);
    printf("Ingrese letra: \n");
}

void client_hangman_run(client_hangman_t* self){
    /*buffer sobre el cual hacer el primer receive, 
    consiguiendo los intentos y el largo de la palabra*/
    char attempts_length_buffer[3]; 
    socket_receive(&self->client_socket, attempts_length_buffer, 3);
    unsigned char attempts = attempts_length_buffer[0];
    //Conversion a decimal de los bits que indican el largo
    uint16_t length = attempts_length_buffer[1] * (16*16) 
                        + attempts_length_buffer[2];
    char partial_word[WORD_MAX_LENGTH];
    socket_receive(&self->client_socket, partial_word, length);
    partial_word[length] = '\0';
    print_info(partial_word, attempts);
    //Mientras el primer bit de attempts sea 0
    while (attempts < 128){ 
        char buffer[INPUT_MAX_SIZE];
        if (!(fgets(buffer, INPUT_MAX_SIZE, stdin))){
            printf("Error at reading input with fgets()");
            return;
        }
        int input_length = strlen(buffer) - 1; //Para no contar el \n
        int i;
        for (i = 0; i < input_length; i++){
            socket_send(&self->client_socket, &buffer[i], 1);
            socket_receive(&self->client_socket, attempts_length_buffer, 3);
            attempts = attempts_length_buffer[0];
            socket_receive(&self->client_socket, partial_word, length);
            if (attempts < 128){
                print_info(partial_word, attempts);
            }else{
                break;
            }
        }
    }
    if (attempts - 128 == 0){
        printf("Perdiste! La palabra secreta era: '%s'\n", partial_word);
    }else{
        printf("Ganaste!!\n");
    }
}

