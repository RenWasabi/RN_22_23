#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> // for memset
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define BACKLOG 20 // how many incoming requests can be queued before accept()
//------------------------------DELETE----------------------------------------------------------------------
//just for testing rn -> i need to somehow fill the struct addrinfo wwith the function getaddrinfo()
//this try has no error handling whatsoever

int main(int argc, char** argv) {
    // Start here :)
    struct addrinfo hints, *result;     //result is a pointer to the resulting addrinfo
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // TCP streaming socket
    hints.ai_flags = AI_PASSIVE; 

    //using getaddrinfo() so we have the stuff ready for upcoming functions
    //MISSING: Errorhandling
    rv = getaddrinfo(NULL, argv[1], &hints, &result);


    //socket()
    int sockfd;
    sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    //bind()
    bind(sockfd, result->ai_addr, result->ai_addrlen);

    //listen()
    listen(sockfd, BACKLOG);
    freeaddrinfo(result); // free list of getaddrinfo results



    return EXIT_SUCCESS;
}
