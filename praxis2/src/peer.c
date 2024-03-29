#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hash_table.h"
#include "neighbour.h"
#include "packet.h"
#include "requests.h"
#include "server.h"
#include "util.h"
#include "scratch1.h" // added by me for testing

// actual underlying hash table
htable **ht = NULL;
rtable **rt = NULL;

// chord peers
peer *self = NULL;
peer *pred = NULL;
peer *succ = NULL;

/**
 * @brief Forward a packet to a peer.
 *
 * @param peer The peer to forward the request to
 * @param pack The packet to forward
 * @return int The status of the sending procedure
 */
int forward(peer *p, packet *pack) {
    /* TODO IMPLEMENT */
    //printf("Goal: forward packet from Peer %d to Peer %d\n", self->node_id, p->node_id);
    // send the lookup packet to the successor
    size_t packet_size; // will store the size of the packet returned by serialize
    unsigned char* serialized_pack = packet_serialize(pack, &packet_size);

    // open a connection to the sucessor peer to forward to
    if (peer_connect(p) < 0){
        printf("An error occured while trying to connect to the successor.\n");
        return -1;
    }

    // s apparantly needs to be the socket of the successor to work
    if (sendall(p->socket, serialized_pack, packet_size) < 0){
        printf("An error occurred while trying to send forward a packet.\n");
        return -1;
    }

    /* in case of proxy request, connection needs to remain open for reply */
    if (pack->flags & PKT_FLAG_CTRL){
            peer_disconnect(p);
        }
    return 0;
}

/**
 * @brief Forward a request to the successor.
 *
 * @param srv The server
 * @param csocket The scokent of the client
 * @param p The packet to forward
 * @param n The peer to forward to
 * @return int The callback status
 */
int proxy_request(server *srv, int csocket, packet *p, peer *n) {
    /* TODO IMPLEMENT */
    forward(n, p);
    char* answer_serial;
    size_t answer_len;
    answer_serial = recvall(n->socket, &answer_len);
    sendall(csocket, answer_serial, answer_len);
    peer_disconnect(n);
    return CB_REMOVE_CLIENT; // the default
    //return CB_OK;
}

/**
 * @brief Lookup the peer responsible for a hash_id.
 *
 * @param hash_id The hash to lookup
 * @return int The callback status
 */
int lookup_peer(uint16_t hash_id) {
    /* TODO IMPLEMENT */
    // create a lookup control packet
    packet* lkup_packet = packet_new(); // initialize packet
    lkup_packet->flags = 0 | PKT_FLAG_CTRL | PKT_FLAG_LKUP; // reserved bits set to 0 // htons????
    // without NBO
    lkup_packet->hash_id = hash_id;
    lkup_packet->node_id = self->node_id;
    lkup_packet->node_ip = peer_get_ip(self);
    lkup_packet->node_port = self->port;

    // send lookup packet to successor -> forward packet
    if (forward(succ, lkup_packet) < 0){
        return -1;
    }
    return 0;
}

/**
 * @brief Handle a client request we are resonspible for.
 *
 * @param srv The server
 * @param c The client
 * @param p The packet
 * @return int The callback status
 */
int handle_own_request(server *srv, client *c, packet *p) {
    /* TODO IMPLEMENT */
    // initialize ACK reply
    packet* ack_packet = packet_new();
    // determine request type and execute
    uint8_t get_flag = p->flags | PKT_FLAG_GET;
    uint8_t set_flag = p->flags | PKT_FLAG_SET;
    if (p->flags == get_flag) {// equal to itself with GET flag set
        // GET request
        ack_packet->flags = 0 | PKT_FLAG_GET | PKT_FLAG_ACK;
        //ack_packet->flags = htons(ack_packet->flags);
        htable* entry = htable_get(ht, p->key, p->key_len);
        if (entry != NULL){
            ack_packet->key = malloc(entry->key_len);
            strncpy(ack_packet->key, entry->key, entry->key_len); // do we need NBO here???
            ack_packet->key_len = entry->key_len;
            ack_packet->value = malloc(entry->value_len);
            strncpy(ack_packet->value, entry->value, entry->value_len);
            ack_packet->value_len = entry->value_len;
        }
    }
    else if (p->flags == set_flag){
            // SET request
            ack_packet->flags = 0 | PKT_FLAG_SET | PKT_FLAG_ACK;
            htable_set(ht, p->key, p->key_len, p->value, p->value_len);
            htable* test_entry;
            HASH_FIND(hh, *(ht), p->key, p->key_len, test_entry);
        }
    else if (p->flags == p->flags | PKT_FLAG_DEL){
        // DEL request
        ack_packet->flags = 0 | PKT_FLAG_DEL | PKT_FLAG_ACK;
        htable_delete(ht, p->key, p->key_len);
    } else {
        // invalid
        printf("Ivalid request!\n"); // ?? HOW should we handle these?
    }

    // send the ACK packet to the client
    size_t packet_size; // will store the size of the packet returned by serialize
    unsigned char* serialized_pack = packet_serialize(ack_packet, &packet_size);
    if (sendall(c->socket, serialized_pack, packet_size) < 0){
        printf("An error occurred while trying to send an ACK packet.\n");
    }
    return CB_REMOVE_CLIENT; // the default
    //return CB_OK;
}

/**
 * @brief Answer a lookup request from a peer.
 *
 * @param p The packet
 * @param n The peer
 * @return int The callback status
 */
int answer_lookup(packet *p, peer *n) {
    /* TODO IMPLEMENT */
    // NOTE: when answer_lookup is triggered, the responsible peer is in the n pointer
    // create a reply control packet
    packet* rply_packet = packet_new(); // initialize packet
    rply_packet->flags = 0 | PKT_FLAG_CTRL | PKT_FLAG_RPLY; // reserved bits set to 0
    // no NBO
    rply_packet->hash_id = p->hash_id;
    rply_packet->node_id = n->node_id;
    rply_packet->node_ip = peer_get_ip(n);
    rply_packet->node_port = n->port;

    // create the peer to send the reply to from the information in the lookup packet
    peer* asking_peer = peer_from_packet(p);
    // send the reply to this peer
    if (forward(asking_peer, rply_packet) < 0){
        printf("Error while forwarding reply packet to peer initiating lookup.\n");
    }

    return CB_REMOVE_CLIENT;
}


/**
 * @brief Handle a key request request from a client.
 *
 * @param srv The server
 * @param c The client
 * @param p The packet
 * @return int The callback status
 */
int handle_packet_data(server *srv, client *c, packet *p) {
    // Hash the key of the <key, value> pair to use for the hash table
    uint16_t hash_id = pseudo_hash(p->key, p->key_len);
    fprintf(stderr, "Hash id: %d\n", hash_id);

    // Forward the packet to the correct peer
    if (peer_is_responsible(pred->node_id, self->node_id, hash_id)) {
        // We are responsible for this key
        fprintf(stderr, "We are responsible.\n");
        return handle_own_request(srv, c, p);
    } else if (peer_is_responsible(self->node_id, succ->node_id, hash_id)) {
        // Our successor is responsible for this key
        fprintf(stderr, "Successor's business.\n");
        return proxy_request(srv, c->socket, p, succ);
    } else {
        // We need to find the peer responsible for this key
        fprintf(stderr, "No idea! Just looking it up!.\n");
        add_request(rt, hash_id, c->socket, p);
        lookup_peer(hash_id);
        return CB_OK;
    }
}

/**
 * @brief Handle a control packet from another peer.
 * Lookup vs. Proxy Reply
 *
 * @param srv The server
 * @param c The client
 * @param p The packet
 * @return int The callback status
 */
int handle_packet_ctrl(server *srv, client *c, packet *p) {

    fprintf(stderr, "Handling control packet...\n");

    if (p->flags & PKT_FLAG_LKUP) {
        // we received a lookup request
        if (peer_is_responsible(pred->node_id, self->node_id, p->hash_id)) {
            // Our business
            fprintf(stderr, "Lol! This should not happen!\n");
            return answer_lookup(p, self);
        } else if (peer_is_responsible(self->node_id, succ->node_id,
                                       p->hash_id)) {
            return answer_lookup(p, succ);
        } else {
            // Great! Somebody else's job!
            forward(succ, p);
        }
    } else if (p->flags & PKT_FLAG_RPLY) {
        // Look for open requests and proxy them
        peer *n = peer_from_packet(p);
        for (request *r = get_requests(rt, p->hash_id); r != NULL;
             r = r->next) {
            proxy_request(srv, r->socket, r->packet, n);
            server_close_socket(srv, r->socket);
        }
        clear_requests(rt, p->hash_id);
    } else {
    }
    return CB_REMOVE_CLIENT;
}

/**
 * @brief Handle a received packet.
 * This can be a key request received from a client or a control packet from
 * another peer.
 *
 * @param srv The server instance
 * @param c The client instance
 * @param p The packet instance
 * @return int The callback status
 */
int handle_packet(server *srv, client *c, packet *p) {
    if (p->flags & PKT_FLAG_CTRL) {
        return handle_packet_ctrl(srv, c, p);
    } else {
        return handle_packet_data(srv, c, p);
    }
}

/**
 * @brief Main entry for a peer of the chord ring.
 *
 * Requires 9 arguments:
 * 1. Id
 * 2. Hostname
 * 3. Port
 * 4. Id of the predecessor
 * 5. Hostname of the predecessor
 * 6. Port of the predecessor
 * 7. Id of the successor
 * 8. Hostname of the successor
 * 9. Port of the successor
 *
 * @param argc The number of arguments
 * @param argv The arguments
 * @return int The exit code
 */
int main(int argc, char **argv) {

    if (argc < 10) {
        fprintf(stderr, "Not enough args! I need ID IP PORT ID_P IP_P PORT_P "
                        "ID_S IP_S PORT_S\n");
    }

    // Read arguments for self
    uint16_t idSelf = strtoul(argv[1], NULL, 10);
    char *hostSelf = argv[2];
    char *portSelf = argv[3];

    // Read arguments for predecessor
    uint16_t idPred = strtoul(argv[4], NULL, 10);
    char *hostPred = argv[5];
    char *portPred = argv[6];

    // Read arguments for successor
    uint16_t idSucc = strtoul(argv[7], NULL, 10);
    char *hostSucc = argv[8];
    char *portSucc = argv[9];

    // Initialize all chord peers
    self = peer_init(
        idSelf, hostSelf,
        portSelf); //  Not really necessary but convenient to store us as a peer
    pred = peer_init(idPred, hostPred, portPred); //

    succ = peer_init(idSucc, hostSucc, portSucc);

    // Initialize outer server for communication with clients
    server *srv = server_setup(portSelf);
    if (srv == NULL) {
        fprintf(stderr, "Server setup failed!\n");
        return -1;
    }
    // Initialize hash table
    ht = (htable **)malloc(sizeof(htable *));
    // Initiale reuqest table
    rt = (rtable **)malloc(sizeof(rtable *));
    *ht = NULL;
    *rt = NULL;

    srv->packet_cb = handle_packet;
    // added by me for testing BEGIN
    //test_peer_inside();

    //peer_test(ht);
    // TEST END
    server_run(srv);
    close(srv->socket);
}


// sudo -H pip install --force-reinstall rnvs_tb-2022_projekt2_1-py3-none-any.whl
// sudo rnvs-tb-dht -s .