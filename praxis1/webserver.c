#include <stdlib.h>
#include <stdio.h> // for testing
#include <errno.h> // for recognizing errors
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //VL
#include <netdb.h> // man pages
#include <string.h> // for memset
#include "listen_respond.h"git
#include "helper_functions.h"


/* cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
 * make -C cmake-build-debug
 * ./cmake-build-debug/webserver
 * */

int main(int argc, char** argv) {
    // Start here :)


    // get port parameter
    char* port_string = argv[1];
    /*
    unsigned short port = port_string_to_ul(port_string);
    printf("Port number: %d\n", port);
     */

    // contains the flag info to feed into getaddrinfo
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 and IPv6 both ok
    hints.ai_socktype = SOCK_STREAM; // make TCP socket
    hints.ai_flags = AI_PASSIVE; // refer to our own id -> listener socket

    printf("test1\n");
    // the linked list returned by getaddrinfo() will be saved here
    struct addrinfo** res;

    // Müssen wir hier schon mit ston die Portnummer konvertieren?
    int gai_error_code = getaddrinfo(NULL, port_string, &hints, res);
    printf("test1a\n");
    if (gai_error_code == 0){
        printf("gai ok\n");
    }
    else {
        printf("getaddrinfo returns %s\n", gai_strerror(gai_error_code));
    }
    printf("%d\n",(*res)->ai_flags);

    printf("test2\n");
    //struct gai_result = (*res)->ai_

    int listener_socket;
    if ((listener_socket = socket((*res)->ai_family, (*res)->ai_socktype, 0)) < 0){
        printf("Error calling socket()\n");
        exit(1);
    }
    else{
        printf("listener_socket: %d\n", listener_socket);
    }
    /*
    int listener_socket2;
    if ((listener_socket2 = socket((*res)->ai_family, (*res)->ai_socktype, 0)) < 0){
        printf("Error calling socket()\n");
        exit(1);
    }
    else{
        printf("listener_socket2: %d\n", listener_socket2);
    }
     */


    /*
    if (bind(listener_socket, (*res)->ai_addr, (*res)->ai_addrlen) != 0){
        printf("Error calling bind()\n");
        exit(1);
    }
    else{
        printf("bind() successful\n");
    }
     */

    printf("test0\n");
    struct sockaddr* socket_address= (*res)->ai_addr;
    int socket_address_len = (*res)->ai_addrlen;
    printf("Test1");
    printf("bind: %d\n", bind(listener_socket, socket_address, socket_address_len) );
    printf("Test2");
    printf("sockaddr: %p\n", socket_address);
    printf("Test3");
    printf("sockaddr->sa_family:%d\n", socket_address->sa_family);
    printf("Test4");
    printf("sockaddr_len:%d\n", socket_address_len);










    //listen_socket(port);


    return EXIT_SUCCESS;
}
