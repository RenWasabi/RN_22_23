#ifndef PRAXIS1_HELPER_FUNCTIONS_H
#define PRAXIS1_HELPER_FUNCTIONS_H

#include "helper_functions.h"
#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h

/* converts the string containing the port number to an
 * unsigned short, exits program with error message if
 * the input is not a valid number */
unsigned short port_string_to_ul(char* port_string);

#endif //PRAXIS1_HELPER_FUNCTIONS_H
