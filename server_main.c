#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "server_hangman.h"

#define SUCCESS 0
#define ERROR 1

int main(int argc, char* argv[]){
    FILE* file = NULL;
    char attempts = 0;
    char* service;
    if (argc > 3){
        file = fopen(argv[3], "r");
        attempts = atoi(argv[2]);
        service = argv[1];
    }
    if (!file || attempts > 127 || attempts < 1){
        return ERROR;
    }
    server_hangman_t server;
    server_hangman_init(&server, file, attempts, 0, service);
    server_hangman_play(&server);
    server_hangman_uninit(&server);
    return SUCCESS;
}
