#include "hash_table.h"

htable* create_entry2(unsigned char* key, uint16_t key_len, unsigned char* value, uint32_t value_len){
    /* allocate space and initialize values (not hh)
     * NOTE: DO NOT USE strlen() FOR KEY AND VALUE!
     * THEY ARE NOT NULL-TERMINATED! PASS AS PARAMETER!
     * */
    htable* entry;
    entry = malloc(sizeof(*entry));
    entry->key = malloc(key_len);
    memcpy(entry->key, key, key_len);
    entry->key_len = key_len;
    entry->value = malloc(value_len);
    memcpy(entry->value, value, value_len);
    entry->value_len = value_len;
    return entry;
}


void htable_set(htable **ht, const unsigned char *key, size_t key_len,
                const unsigned char *value, size_t value_len) {
    /* TODO IMPLEMENT */
    // check if an entry associated with this key exists already
    // HASH_FIND(hh, head, keyptr, keylen, out)
    // nicht einfacher:  HASH_FIND_STR(users, "betty", s);??
    htable* entry;
    HASH_FIND(hh, *(ht), &key, key_len, entry);
    // entry exists -> overwrite the value
    if (entry != NULL){
        printf("There is an entry\n");
        free(entry->value);
        entry->value = malloc(key_len*sizeof(unsigned char));
        memcpy(entry->value, value, value_len);
        return;
    }
    printf("no entry\n");
    // entry doesn't exist -> create a new entry
    entry = create_entry2(key, key_len, value, value_len);
    //print_table_entry(entry);
    printf("is entry there? %d\n", entry != NULL);
    printf("Key len value len: %d %d\n", entry->key_len, entry->value_len);
    // add it to the hash
    HASH_ADD_KEYPTR(hh, *(ht), &key, key_len, entry);
    htable* entry_test = NULL;
    HASH_FIND(hh, *(ht), &key, key_len, entry_test);
    printf("Was an entry made? %d\n", entry_test != NULL);
    printf("Was no entry made? %d\n", entry_test == NULL);
    printf("Test key value len: %d %d\n", entry_test->key_len, entry_test->value_len);
}

htable *htable_get(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
    htable* entry;
    HASH_FIND(hh, *(ht), &key, key_len, entry);
    if (entry == NULL){
        // no entry with this key exists in hash table
        printf("GET can'T find anything.\n");
        return NULL;
    }
    // entry exists
    printf("GET found smth\n");
    return entry;
}

int htable_delete(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
}


