#include "hash_table.h"



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

// END HELPER

void htable_set(htable **ht, const unsigned char *key, size_t key_len,
                const unsigned char *value, size_t value_len) {
    /* TODO IMPLEMENT */
    // check if an entry associated with this key exists already
    // HASH_FIND(hh, head, keyptr, keylen, out)
    // nicht einfacher:  HASH_FIND_STR(users, "betty", s);??
    htable* entry = NULL;
    HASH_FIND(hh, ht, key, key_len, entry);
    // entry exists -> overwrite the value
    if (entry != NULL){
        free(entry->value);
        entry->value = malloc(keylen*sizeof(unsigned char));
        strncpy(entry->value, value, value_len);
        return;
    }
    // entry doesn't exist -> create a new entry
    entry = create_entry(key, value);
    // add it to the hash
    HASH_ADD_KEYPTR(hh, ht, key, key_len, entry);

}

htable *htable_get(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
}

int htable_delete(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
}



