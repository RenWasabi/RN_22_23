#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> // for memset


int main(int argc, char** argv) {
    // Start here :)
    /* ACHTUNG: Beej's Guide to Network Programming wurde als Referenz benutzt,
     * während Kopien vermieden wurden, können dennoch einige Stellen
     * Ähnlichkeit haben. */

    // initialize the addrinfo structure for the listening socket
    struct addrinfo hints;
    struct addrinfo *listener_addrinfo;

    memset(&hints, 0, sizeof(hints)); // without initializing with 0, hints might contain unexpected values
    // ACHTUNG: Die folgenden drei Zeilen direkt aus dem Tutorium übernommen.
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 both ok
    hints.ai_socktype = SOCK_STREAM; // TCP streaming socket
    hints.ai_flags = AI_PASSIVE; // socket for listening on machine's ip

    // MISSING ERROR CHECKING for invalid argv[1] input
    int gai_error_code = getaddrinfo(NULL, argv[1], &hints, &listener_addrinfo);
    if (gai_error_code != 0){
        printf("Error calling getaddrinfo: %s\n", gai_strerror(gai_error_code));
        exit(1);
    }

    // create listener socket
    // MISSING iterating over linked list starting add listener_addrinfo in case first result doesn't work
    int listener_fd = socket(listener_addrinfo->ai_family, listener_addrinfo->ai_socktype, listener_addrinfo->ai_protocol);
    if (listener_fd < 0){
        printf("Error caling socket()\n");
        exit(1);
    }
    printf("Created Listener Socket. Socket-ID: %d\n", listener_fd);

    // bind listener socket to port




    freeaddrinfo(listener_addrinfo); // free list of getaddrinfo results



    return EXIT_SUCCESS;
}
