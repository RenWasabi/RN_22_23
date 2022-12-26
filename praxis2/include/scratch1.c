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

int client_test(){
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