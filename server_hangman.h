#ifndef SERVER_HANGMAN_H
#define SERVER_HANGMAN_H

#include <stdio.h>
#include <stdlib.h>
#include "common_socket.h"

typedef struct server_hangman{
    FILE* words_file;
    socket_t host_socket;
    unsigned char attempts;
    int wins;
    int loses;
}server_hangman_t;


void server_hangman_init(server_hangman_t* server,
                        FILE* words_file,
                        unsigned char attempts,
                        const char *host,
                        const char *service);

void server_hangman_uninit(server_hangman_t* server);

void server_hangman_play(server_hangman_t* server);

#endif
