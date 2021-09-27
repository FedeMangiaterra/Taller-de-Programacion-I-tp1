#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "common_socket.h"

#define MAX_QUEUE 15 //El valor que se va a usar en listen()
#define SUCCESS 0
#define ERROR 1

int socket_init(socket_t* self){
    self->fd = socket(AF_INET, SOCK_STREAM, 6);
    if (self->fd == -1){
        printf("Couldn't create socket \n");
        return ERROR;
    }
    return SUCCESS;
}

void socket_uninit(socket_t* self){
    close(self->fd);
}

int socket_bind_and_listen(socket_t *self,
                            const char *host,
                            const char *service){
    struct addrinfo hints;
    struct addrinfo *ptr, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(host, service, &hints, &res) < 0){
        printf("Error at getaddrinfo() \n");
        return ERROR;
    }
    ptr = res;
    int val = 1;
    if (setsockopt(self->fd,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &val,
                    sizeof(val)) == -1){
        printf("Error at stsockopt()\n");
        return ERROR;
    }
    if (bind(self->fd, ptr->ai_addr, ptr->ai_addrlen) < 0){
        printf("Error at bind() \n");
        freeaddrinfo(res);
        return ERROR;
    }
    freeaddrinfo(res);
    listen(self->fd, MAX_QUEUE);
    return SUCCESS;
}

int socket_connect(socket_t *self, const char *host, const char *service){
    struct addrinfo hints;
    struct addrinfo *ptr, *res;
    bool connection_established = false;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = 0;
    if (getaddrinfo(host, service, &hints, &res) < 0){
        printf("Error at getaddrinfo() \n");
        return ERROR;
    }
    ptr = res;
    if (connect(self->fd, ptr->ai_addr, ptr->ai_addrlen) < 0){
            printf("Failed to connect \n");
        }else{
            connection_established = true;
        }
    freeaddrinfo(res);
    if (!connection_established) return ERROR;
    return SUCCESS;
}

int socket_accept(socket_t *listener, socket_t *peer){
    peer->fd = accept(listener->fd, NULL, NULL);
    if (peer->fd == -1){
        printf("Error at accept \n");
        return ERROR;
    }
    return SUCCESS;
}

ssize_t socket_send(socket_t *self, const char *buffer, size_t length){
    ssize_t total_bytes_sent = 0;
    while (total_bytes_sent < length){
        int bytes_sent = send(self->fd,
                             &buffer[total_bytes_sent],
                            length - total_bytes_sent,
                            MSG_NOSIGNAL);
        if (bytes_sent == 0){
            printf("Error: socket is closed \n");
            return -1;
        }else if (bytes_sent == -1){
            printf("Error at send(): %s\n", gai_strerror(bytes_sent));
            return -1;
        }else{
            total_bytes_sent += bytes_sent;
        }
    }
    return total_bytes_sent;
}

ssize_t socket_receive(socket_t *self, char *buffer, size_t length){
    ssize_t total_bytes_received = 0;
    while (total_bytes_received < length){
        int bytes_received = recv(self->fd,
                                 &buffer[total_bytes_received],
                                length - total_bytes_received,
                                0);
        if (bytes_received == 0){
            printf("Error: socket is closed \n");
            return -1;
        }else if (bytes_received == -1){
            printf("Error at receive(): %s\n", gai_strerror(bytes_received));
            return -1;
        }else{
            total_bytes_received += bytes_received;
        }
    }
    return total_bytes_received;
}
