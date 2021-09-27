#ifndef CLIENT_HANGMAN_H
#define CLIENT_HANGMAN_H

#include "common_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct client_hangman{
    socket_t client_socket;
}client_hangman_t;

void client_hangman_init(client_hangman_t* self,
                         const char *host,
                        const char *service);

void client_hangman_uninit(client_hangman_t* self);

void client_hangman_run(client_hangman_t* self);

#endif
