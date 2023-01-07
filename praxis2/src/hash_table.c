#include "hash_table.h"

htable* create_entry2(const unsigned char* key, uint16_t key_len, const unsigned char* value, uint32_t value_len){
    /* allocate space and initialize values (not hh)
     * NOTE: DO NOT USE strlen() FOR KEY AND VALUE!
     * THEY ARE NOT NULL-TERMINATED! PASS AS PARAMETER!
     * */
    htable* entry;
    entry = malloc(sizeof(entry));
    //entry->key = key;
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
    HASH_FIND(hh, *(ht), key, key_len, entry);
    // entry exists -> overwrite the value
    if (entry != NULL){
        printf("Existing entry for this key.\n");
        HASH_DEL(*(ht), entry);
        free(entry->value);
        free(entry->key);
        free(entry);
    }
    else {
        printf("No existing entry\n");
    }
    // entry doesn't exist -> create a new entry
    //entry = create_entry2(key, key_len, value, value_len);
    entry = malloc(sizeof(htable));
    entry->key = malloc(key_len);
    entry->value = malloc(value_len);
    memcpy(entry->key, key, key_len);
    memcpy(entry->value, value, value_len);
    entry->key_len = key_len;
    entry->value_len = value_len;
    // add it to the hash
    HASH_ADD_KEYPTR(hh, *(ht), entry->key, entry->key_len, entry);
    printf("finished SETting the enty?\n");
    return;
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
    htable* toDelete;
    HASH_FIND(hh, *(ht), key, key_len, toDelete);
    HASH_DEL(*(ht), toDelete);
    
    //freeing everything
    free(toDelete->key);
    free(toDelete->key_len);
    free(toDelete->value);
    free(toDelete->value_len);
   
    free(toDelete);
}


