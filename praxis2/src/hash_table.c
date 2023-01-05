#include "hash_table.h"

void htable_set(htable **ht, const unsigned char *key, size_t key_len,
                const unsigned char *value, size_t value_len) {
    /* TODO IMPLEMENT */
    // check if an entry associated with this key exists already
    // HASH_FIND(hh, head, keyptr, keylen, out)
    // nicht einfacher:  HASH_FIND_STR(users, "betty", s);??
    htable* entry = NULL;
    HASH_FIND(hh, *(ht), key, key_len, entry);
    // entry exists -> overwrite the value
    if (entry != NULL){
        free(entry->value);
        entry->value = malloc(key_len*sizeof(unsigned char));
        strncpy(entry->value, value, value_len);
        return;
    }
    // entry doesn't exist -> create a new entry
    entry = create_entry(key, key_len, value, value_len);
    // add it to the hash
    HASH_ADD_KEYPTR(hh, *(ht), key, key_len, entry);
}

htable *htable_get(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
    htable* entry = NULL;
    HASH_FIND(hh, *(ht), key, key_len, entry);
    if (entry == NULL){
        // no entry with this key exists in hash table
        return NULL;
    }
    // entry exists
    return entry;
}

int htable_delete(htable **ht, const unsigned char *key, size_t key_len) {
    /* TODO IMPLEMENT */
}
