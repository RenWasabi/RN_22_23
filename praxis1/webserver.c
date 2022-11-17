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

    // MISSING ERROR CHECKING for invalid argv[1] input (both if number and if range is that of short)
    int gai_error_code = getaddrinfo(NULL, argv[1], &hints, &listener_addrinfo);
    if (gai_error_code != 0){
        printf("Error calling getaddrinfo: %s\n", gai_strerror(gai_error_code));
        exit(1);
    }

    /*
    struct addrinfo* gai_tmp;
    for (gai_tmp = listener_addrinfo; gai_tmp != NULL; gai_tmp = gai_tmp->ai_next){
        void* addr;
        char* ipver;
        uint16_t port_nr;
        struct sockaddr *listener_sockaddr = gai_tmp->ai_addr;
        if (listener_addrinfo->ai_family == AF_INET){
            struct sockaddr_in* listen_ipv4 =(struct sockaddr_in*) listener_sockaddr;
            addr = &(listen_ipv4->sin_addr);
            ipver = "IPv4";
            port_nr = ntohs(listen_ipv4->sin_port); // convert port from network byte order to host
        }
        else {
            struct sockaddr_in6* listen_ipv6 = (struct sockaddr_in6*) listener_sockaddr;
            addr = &(listen_ipv6->sin6_addr);
            ipver = "IPv6";
            port_nr = ntohs(listen_ipv6->sin6_port); // convert port from network byte order to host
        }

        int ip_buf_len = 50;
        char* ip_address = calloc(ip_buf_len,sizeof(char));
        inet_ntop(gai_tmp->ai_family, addr, ip_address, ip_buf_len);
        printf("%s: %s\n", ipver, ip_address);
        printf("corresponding port: %d\n", port_nr);
        // END OF IP-ADDRESS TESTING BLOCK
        }
        */

    // create listener socket
    // MISSING iterating over linked list starting add listener_addrinfo in case first result doesn't work
    int listener_fd = socket(listener_addrinfo->ai_family, listener_addrinfo->ai_socktype, listener_addrinfo->ai_protocol);
    if (listener_fd < 0){
        printf("Error calling socket()\n");
        exit(1);
    }

    // END OF IP-ADDRESS TESTING BLOCK
    /* This code prints the IP-Address associated with the listener socket.
     * It's for testing purposes.
     * Except for some deviation in the naming, the code is directly taken from
     * Beej's Guide to Network Programming, section 3.4 IP Addresses, Part Deux */
    /*
    void* addr;
    char* ipver;
    struct sockaddr *listener_sockaddr = listener_addrinfo->ai_addr;
    if (listener_addrinfo->ai_family == AF_INET){
        struct sockaddr_in* listen_ipv4 =(struct sockaddr_in*) listener_sockaddr;
        addr = &(listen_ipv4->sin_addr);
        ipver = "IPv4";
    }
    else {
        struct sockaddr_in6* listen_ipv6 = (struct sockaddr_in6*) listener_sockaddr;
        addr = &(listen_ipv6->sin6_addr);
        ipver = "IPv6";
    }

    int ip_buf_len = 50;
    char* ip_address = calloc(ip_buf_len,sizeof(char));
    inet_ntop(listener_addrinfo->ai_family, addr, ip_address, ip_buf_len);
    printf("%s: %s\n", ipver, ip_address);
    // END OF IP-ADDRESS TESTING BLOCK
     */


    // bind listener socket to port
    errno = 0;
    if (bind(listener_fd, listener_addrinfo->ai_addr, listener_addrinfo->ai_addrlen) < 0){
        printf("Error calling bind()\n");
        printf("errno after bind: %d\n", errno);
        perror("Error meaning: ");
        exit(1);
    }

    errno = 0;
    if (listen(listener_fd, BACKLOG) < 0){
        printf("Error calling listen()");
        perror("Error: ");
        exit(1);
    }
    // IDEALLY: replace argv[1] by the actual sin_port/sin6_port
    printf("Socket %d listening on port %s.\n", listener_fd, argv[1]);


    // for storing information about the client (storage is IP version agnostic)
    struct sockaddr_storage client_addr;
    socklen_t client_addrlen;
    errno = 0;
    // the following line is again taken from Beej's Guide to Network Programming
    int connection_fd = accept(listener_fd, (struct sockaddr*)&client_addr, &client_addrlen);
    if (connection_fd < 0){
        perror("Error on calling accept(): ");
        exit(1);
    }
    printf("Accepted request on new socket: %d\n", connection_fd);





    freeaddrinfo(listener_addrinfo); // free list of getaddrinfo results



    return EXIT_SUCCESS;
}
