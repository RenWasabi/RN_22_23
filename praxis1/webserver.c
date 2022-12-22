#include <stdlib.h>
#include <stdio.h> // for testing
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> // for memset
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h> // for closing file descriptors with close()

#define BACKLOG 20 // how many incoming requests can be queued before accept()
#define BUFFER_SIZE 100 // size of the char buffer that recv() writes client requests to

/* cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
 * make -C cmake-build-debug
 * ./cmake-build-debug/webserver
 * */

typedef struct buf_struct {
    char* buffer; // the actual buffer
    int available_space; // size of buffer in bytes
    int used_space; // number of valid bytes (from beginning of buffer)
    int scanner_index; // last position at which the buffer has been scanned for terminator sequence
    int req_begin_index; // beginning of current request in buffer, always 0 for the secondary buffer
} buf_struct;

// create and initialize an administration structure for a buffer, including the buffer itself
buf_struct* buf_struct_init(int buf_size){
    if (buf_size <= 0){
        fprintf(stderr, "Buffer can not be created with size %d\n", buf_size);
        exit(1);
    }
    buf_struct* buf_struct = malloc(sizeof(buf_struct));
    buf_struct->buffer = malloc(buf_size* sizeof(char)); // missing error handling
    memset(buf_struct->buffer, 0, buf_size);
    buf_struct->available_space = buf_size;
    buf_struct->used_space = 0;
    buf_struct->scanner_index = 0;
    buf_struct->req_begin_index = 0;
    printf("Created buf_struct with buffer size %d at address %p.\n", buf_struct->available_space, buf_struct);
    return buf_struct;
}

void buf_struct_free(buf_struct* buf_to_free){
    free(buf_to_free->buffer);
    free(buf_to_free);
    return;
}


buf_struct* buf_struct_expand(buf_struct* old_buf){
    buf_struct* new_buf = buf_struct_init(old_buf->available_space*2);
    strncpy(new_buf->buffer, old_buf->buffer, old_buf->used_space);
    new_buf->used_space = old_buf->used_space;
    new_buf->scanner_index = old_buf->scanner_index;
    // no adjusting of req_begin_index since that's only relevant in the recv buffer, which will not
    // be expanded
    printf("Expanded old buffer at %p with content %s and size %d "
           "to new buffer at %p with content %s and size %d\n.", \
           old_buf, old_buf->buffer, old_buf->available_space, \
        new_buf, new_buf->buffer, new_buf->available_space);
    free(old_buf);
    return new_buf;
}


/* compare whether the end of the valid buffer matches a specified terminator sequene
 * valid buffer is specified by how many bytes were received (=> written to buffer)
 * with recv() */
// REDUNDANT! terminator not only at the end of received buffer if several requests
// received at once
int cmp_buffend_terminator(char* buffer, int valid_bytes, char* terminator, int terminator_len){
    // fill last_bytes with the last <terminator_len> bytes copied into the buffer in last recv()
    char* last_bytes = malloc(terminator_len* sizeof(char));
    strncpy(last_bytes, buffer+valid_bytes-terminator_len, terminator_len);
    int strings_same = 0;
    if (strncmp(last_bytes, terminator, terminator_len) == 0){
        strings_same = 1;
    }
    return strings_same;
}

// reply_socket is the socket that the reply will be send to
int process_request(char* request, int req_len, int reply_socket){
    printf("Received request of length %d:\n%s\n", req_len, request);
    char* reply_msg = "Reply\r\n";
    if(send(reply_socket, reply_msg, sizeof(reply_msg)-1, 0) < 0) {
        exit(1);}
    return 0;
}



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
        perror("Error on calling accept() ");
        exit(1);
    }
    printf("Accepted request on new socket: %d\n", connection_fd);

    // recv und send
    // aus dem Beispiel in der VL

    // the main buffer to receive the stream in
    buf_struct* recv_buf = buf_struct_init(BUFFER_SIZE);

    char* http_terminator = "\r\n\r\n";
    int terminator_length = 4; // otherwise the \n will be treated as two symbols

    int recv_buf_fully_processed = 0; // controlling the scan loop of recv buffer

    // OUTER RECV LOOP (PRIMARY BUFFER)
    while(1){
        errno = 0;
        int received_bytes = recv(connection_fd, recv_buf->buffer, recv_buf->available_space, 0);
        recv_buf->used_space = received_bytes;
        // recv() failed
        if (received_bytes < 0) {
            perror("Error while receiving: ");
            close(connection_fd);
            close(listener_fd);
            exit(1);
            // client closed connection
        }
        if (received_bytes == 0){
            printf("The client closed the connection.\n");
            close(connection_fd);
            close(listener_fd);
            exit(1);
        }

        // data was received successfully
        printf("Received %d bytes from client.\n", received_bytes);
        printf("Received: %s\n", recv_buf->buffer);

        int remaining = 0;
        // scan the recv buffer
        RECV_SCAN:
        while(recv_buf->scanner_index <= recv_buf->used_space-terminator_length){

            /* terminator_present == 1:
             * => the terminator sequence is in the recv buffer, starts at scanner index */
            int terminator_present = 0;
            if (strncmp(recv_buf->buffer+recv_buf->scanner_index, http_terminator, terminator_length) == 0){
                terminator_present = 1;
            }

            // process the request if it is not empty
            if (terminator_present == 1){
                if (recv_buf->scanner_index == recv_buf->req_begin_index){
                    fprintf(stderr, "Terminator recognized but not preceded but request.\n");
                    // MISSING ERROR HANDLING
                    close(connection_fd);
                    close(listener_fd);
                    exit(1);
                } else {
                    process_request(recv_buf->buffer, recv_buf->scanner_index-recv_buf->req_begin_index+terminator_length, connection_fd);
                }

                if (recv_buf->scanner_index == recv_buf->used_space-terminator_length){
                    // no next request in same recv possible, no more scanning needed
                    recv_buf_fully_processed = 1;
                } else {
                    // beginning of next request (if present) will be directly after end of current
                    recv_buf->req_begin_index = recv_buf->scanner_index+terminator_length;
                }

            }

            recv_buf->scanner_index++;
        }

        if (recv_buf_fully_processed == 1){
            printf("Recv buf has been fully processed.\n");
            continue; // go on with receiving new
        }

        /* scanned to end of valid buffer, still no terminator detected
         * => store content of buffer in secondary buffer and continue receiving
         * into this one, and scanning it, until full request is recognized */
        printf("Unprocessed content remains in recv buffer, but no request detected.\n");

        // create a secondary buffer for processing this request
        buf_struct* req_buf = buf_struct_init(recv_buf->available_space*2);
        // copy content of recv buffer into req buffer
        strncpy(req_buf->buffer, recv_buf->buffer, recv_buf->used_space);
        req_buf->used_space = recv_buf->used_space;

        // INNER RECV LOOP (SECONDARY BUFFER)
        int terminator_present2 = 0;
        while(1){ // DIFFERENT CONDITION?
            errno = 0;
            int received_bytes = recv(connection_fd, recv_buf->buffer, recv_buf->available_space, 0);
            recv_buf->used_space = received_bytes;
            // recv() failed
            if (received_bytes < 0) {
                perror("Error while receiving: ");
                close(connection_fd);
                close(listener_fd);
                exit(1);
                // client closed connection
            }
            if (received_bytes == 0){
                printf("The client closed the connection.\n");
                close(connection_fd);
                close(listener_fd);
                exit(1);
            }
            // data was received successfully
            printf("Received %d bytes from client.\n", received_bytes);
            printf("Received: %s\n", recv_buf->buffer);
            // test if req_buffer has enough space to fit the newly received content in addition to the old content
            if (req_buf->available_space-req_buf->used_space < received_bytes){
                // not enough space -> expand
                req_buf = buf_struct_expand(req_buf);
            }
            // copy newly received content to the end of req_buf
            strncpy(req_buf->buffer+req_buf->used_space, recv_buf->buffer, recv_buf->used_space);
            req_buf->used_space += recv_buf->used_space;

            // scan req buffer
            while(req_buf->scanner_index <= req_buf->used_space-terminator_length){

                // SHAKY
                if (strncmp(req_buf->buffer+req_buf->scanner_index, http_terminator, terminator_length) == 0){
                    terminator_present2 = 1;
                }
                // process the request if it is not empty
                if (terminator_present2 == 1){
                    if (req_buf->scanner_index == 0){
                        fprintf(stderr, "Terminator recognized but not preceded but request.\n");
                        // MISSING ERROR HANDLING
                        close(connection_fd);
                        close(listener_fd);
                        exit(1);
                    } else {
                        process_request(req_buf->buffer, req_buf->scanner_index+terminator_length+1, connection_fd);
                    }

                    // how much valid content in the buffer does not belong to request? => remaining
                    remaining = req_buf->used_space-(req_buf->scanner_index+terminator_length);
                    int req_buf_fully_processed = 0;
                    if (remaining == 0){
                        req_buf_fully_processed = 1;
                    } else {
                        strncpy(recv_buf->buffer, req_buf->buffer+(req_buf->used_space-remaining), remaining); // OFF BY ONE
                        recv_buf->used_space = remaining;
                    }
                    buf_struct_free(req_buf);
                    break; //no more scanning
                }
            } // end inner scan loop
            /* inner scan loop ended
             * exit inner recv loop, if a request was processed */
            if (terminator_present2 == 1){
                break;
            }
        } // end inner recv loop
        if (remaining != 0){
            goto RECV_SCAN;
        }
        recv_buf->scanner_index = 0;
    } // end outer recv loop


        /* OLD SEND RECV LOOP
        while(1) {
            errno = 0;
            int received_bytes = recv(connection_fd, recv_buf->buffer, recv_buf->available_space, 0);
            // recv() failed
            if (received_bytes < 0) {
                perror("Error while receiving: ");
                exit(1);
            // client closed connection
            } else if (received_bytes == 0){
                printf("The client closed the connection.\n");
                //close(connection_fd);
                exit(1);
            // data was received successfully
            } else {
                printf("Received %d bytes from client.\n", received_bytes);
                printf("Received: %s\n", recv_buf->buffer);
                // test for http packet
                if (received_bytes < terminator_length+1){
                    printf("Too short to be valid HTTP packet.\n");
                } else if (received_bytes >= recv_buf->available_space){
                    printf("Packet fills buffer, requires separate handling.\n");
                } else {
                    //printf("Last 4 Bytes identical to terminator? %d\n",
                    //       cmp_buffend_terminator(buffer, received_bytes, http_terminator, terminator_length) );
                    if (cmp_buffend_terminator(recv_buf->buffer, received_bytes, http_terminator, terminator_length) == 0){
                        printf("Not a valid HTTP request.\n");
                    } else {
                        printf("Valid HTML packet.\n");
                        if(send(connection_fd, reply_msg, sizeof(reply_msg)-1, 0) < 0) {
                            exit(1);}
                    }
                }


            }

        }   */

        //close(listener_fd);



    //freeaddrinfo(listener_addrinfo); // free list of getaddrinfo results
    close(listener_fd);
    close(connection_fd);



    return EXIT_SUCCESS;
}

/* example of a request (can be recv with one recv()
 * that contains more than one HTTP packet
echo -en 'GET / HTTP/1.1\r\nHost:
example.com\r\n\r\n GET / HTTP/1.1\r\nHost:
example.com\r\n\r\n' | nc localhost 80
 */






