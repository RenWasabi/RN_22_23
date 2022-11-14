#include <stdlib.h>
#include <stdio.h> // for testing
#include <errno.h> // for recognizing errors
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h
#include "listen_respond.h"
#include "helper_functions.h"

/* cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
 * make -C cmake-build-debug
 * ./cmake-build-debug/webserver
 * */

int main(int argc, char** argv) {
    // Start here :)


    // get port parameter
    char* port_string = argv[1];
    unsigned short port = port_string_to_ul(port_string);
    printf("Port number: %d\n", port);



    //listen_socket(port);


    return EXIT_SUCCESS;
}
