//
// Created by caro on 26.12.22.
//
#include "scratch1.h" // for linking to executable client/peer file

/*
int main(){

    printf("Test.\n");



    return 0;
}
*/

// HELPER FUNCTIONS

htable* create_entry(unsigned char* key, unsigned char* value){
    /* allocate space and initialize values (not hh)
     * NOTE: use strlen do reserve place for string and determine string size
     * using sizeof seems to always just return the size of the pointer
     * to the string (? 8 byte?) */
    int key_len = strlen(key);
    int value_len = strlen(value);
    htable* entry;
    entry = malloc(sizeof(*entry));
    entry->key = malloc(key_len);
    strncpy(entry->key, key, key_len);
    entry->key_len = key_len;
    entry->value = malloc(value_len);
    strncpy(entry->value, value, value_len);
    entry->value_len = value_len;
    return entry;
}

void print_table_entry(htable* entry){
    printf("Table Entry:\n");
    printf("Key of length %d: %s\n", entry->key_len, entry->key);
    printf("Value of length %d: %s\n", entry->value_len, entry->value);
    int hashv = 0;
    HASH_VALUE(entry->key, entry->key_len, hashv);
    printf("Hash value: %d\n", hashv);
}

void print_packet(packet* packet){
    printf("---------------------------------------------------------\n");
    printf("PACKET:\n");
    printf("Flags: %b\n", packet->flags);
    printf("Keylen: %d\n", ntohs(packet->key_len));
    printf("Value_len: %d\n", ntohs(packet->value_len));
    if (packet->key_len <= 0){
        printf("No key.\n");
    } else {
        printf("Key: %s\n", packet->key);
    }
    if (packet->value_len <= 0){
        printf("No value.\n");
    } else {
        printf("Value: %s\n", packet->value);
    }
    printf("Hash_Id: %d\n", ntohs(packet->hash_id));
    printf("Node_Id: %d\n", ntohs(packet->node_id));
    printf("Node_Ip: %d\n", ntohl(packet->node_ip));
    printf("Node_Port: %d\n", ntohs(packet->node_port));
    printf("---------------------------------------------------------\n");



}

int test_peer_is_responsible(uint16_t pred_id, uint16_t peer_id, uint16_t hash_id, int desired_output){
    if (peer_is_responsible(pred_id, peer_id, hash_id) == desired_output){
        return 1;
    } else {
        return 0;
    }
}

// actually testing with multiple values
int all_tests_peer_is_responsible(){
    int all_ok = 1; // overall result of all tests

    // test 1
    uint16_t pred_id = 138;
    uint16_t peer_id = 202;
    uint16_t hash_id = 111;
    int desired_output = 0;
    int test_result = test_peer_is_responsible(pred_id, peer_id, hash_id, desired_output);
    printf("Test for peer_is_responsible(%d,%d,%d): %d\n", pred_id, peer_id, hash_id, test_result);
    if (test_result == 0){
        all_ok = 0;
    }

    // test 2 (overflow)
    pred_id = 202;
    peer_id = 10;
    hash_id = 111;
    desired_output = 0;
    test_result = test_peer_is_responsible(pred_id, peer_id, hash_id, desired_output);
    printf("Test for peer_is_responsible(%d,%d,%d): %d\n", pred_id, peer_id, hash_id, test_result);
    if (test_result == 0){
        all_ok = 0;
    }

    // test 3
    pred_id = 10;
    peer_id = 74;
    hash_id = 111;
    desired_output = 0;
    test_result = test_peer_is_responsible(pred_id, peer_id, hash_id, desired_output);
    printf("Test for peer_is_responsible(%d,%d,%d): %d\n", pred_id, peer_id, hash_id, test_result);
    if (test_result == 0){
        all_ok = 0;
    }

    // test 4
    pred_id = 74;
    peer_id = 138;
    hash_id = 111;
    desired_output = 1;
    test_result = test_peer_is_responsible(pred_id, peer_id, hash_id, desired_output);
    printf("Test for peer_is_responsible(%d,%d,%d): %d\n", pred_id, peer_id, hash_id, test_result);
    if (test_result == 0){
        all_ok = 0;
    }

    return all_ok;
}

int client_test(){
    // unit tests
    printf("---------------------------------------------------------\n");
    // peer_is_responsible in neighbor
    int test_result = all_tests_peer_is_responsible();
    printf("All tests for peer_is_responsible: %d\n", test_result);
    printf("---------------------------------------------------------\n");




    printf("Test.\n");

    /* initialize hash table as with NULL
     * as pointer to the hash structure */
    htable* htable_test1 = NULL;

    unsigned char* testkey1 = "testkey1";
    unsigned char* testvalue1 = "testvalue1";

    htable* test1_entry1 = create_entry(testkey1, testvalue1);
    print_table_entry(test1_entry1);



    return 0;
}

// for testing functions in a peer process
int peer_test(){
    printf("Test Peer.\n");
    printf("---------------------------------------------------------\n");



    return 0;
}