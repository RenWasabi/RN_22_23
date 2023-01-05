//
// Created by caro on 26.12.22.
//
#include <stdio.h> // for testing
#include "uthash.h" // for hashing
#include "hash_table.h" // for the htable_struct
#include <string.h> // for giving string keys and values as table entry
#include "neighbour.h" // for testing peer_is_responsible()
/* added by IDE
#ifndef RN_PRAXIS2_SCRATCH1_H
#define RN_PRAXIS2_SCRATCH1_H

#endif //RN_PRAXIS2_SCRATCH1_H
*/

// HELPER FUNCTIONS

/* takes a char key and value and returns a pointer to an
 * htable entry created from these */
htable* create_entry(unsigned char* key, uint16_t key_len, unsigned char* value, uint32_t value_len);

// print a table entry
void print_table_entry(htable* entry);

void print_packet(packet* packet);

// HERE
void test_htable_set(htable** ht);

// "MAIN"
/* since client is an executable file I just link
 * whatever short tests I want to perform inside
 */
int client_test();


int peer_test();