#include "listen_respond.h"
#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h


void print_test(char* string){
    printf(string);
    return;
}

int listen_socket(int port){

    /* AF_INET: IP-address family, used in lecture
     * SOCK_STREAM: socket type is TCP (streaming socket) */
    int socket_descriptor = socket(AF_INET, SOCK_STREAM,0);
    if (socket_descriptor < 0) {
        // missing error handling/log
        exit(1);
    }
    /*
    // initialize the struct storing information about the listen socket
    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET; // address family IPv4
    s_addr.sin_port = htons(port); //
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
     */

}
