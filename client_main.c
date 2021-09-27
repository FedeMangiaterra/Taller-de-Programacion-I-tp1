#include "client_hangman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define ERROR 1

int main(int argc, char* argv[]){
    char* host = NULL;
    char* service = NULL;
    if (argc > 2){
        host = argv[1];
        service = argv[2];
    }
    if (host && service){
        client_hangman_t client;
        client_hangman_init(&client, host, service);
        client_hangman_run(&client);
        client_hangman_uninit(&client);
        return SUCCESS;
    }
    return ERROR;
}
