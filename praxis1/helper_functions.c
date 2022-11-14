#include "helper_functions.h"
#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h

unsigned short port_string_to_ul(char* port_string){
    char* invalid_input_message = "Please use a port number between 0 and 65,535 as argument.\n";

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
    return port;
}