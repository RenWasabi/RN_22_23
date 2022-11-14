#include <stdlib.h>
#include <stdio.h> // for testing
#include <errno.h> // for recognizing errors
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h
#include "listen_respond.h"

/* cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
 * make -C cmake-build-debug
 * ./cmake-build-debug/webserver
 * */

int main(int argc, char** argv) {
    // Start here :)


    // get port parameter
    char* invalid_input_message = "Please use a port number between 0 and 65,535 as argument.\n";
    char* port_string = argv[1];
    // missing input
    if (port_string == NULL){
        printf(invalid_input_message);
        exit(1);
    }
    /* convert input string to long
     * strtol_endptr points to the first byte of the string
     * which does not represent a digit => if it's value is the
     * null terminator, the string is a valid numerical argument */
    char** strtol_endptr = (char**) malloc(sizeof(char *));
    int port = strtol(port_string, strtol_endptr, 10);
    if (*(*(strtol_endptr)) != '\0'){
        printf(invalid_input_message);
        exit(1);
    }
    /* check if the input number is within the range that can be
     * used as a port number (i.e. that of unsigned short) */
    if (port < 0 || port > 65535 ){
        printf(invalid_input_message);
        exit(1);
    }
    // cast long to unsigned short (port number format)
    port = (unsigned short) port;
    printf("Port number: %d\n", port);



    //listen_socket(port);


    return EXIT_SUCCESS;
}
