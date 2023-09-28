#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "concurrent_hash_map_gap.h"


// A structure to represent a key-value pair
uint64_t hash(const uint64_t key) {
    return key % (BUCKEDTS_SIZE);
}


// A function to create a new hash map
map_t *create_map() {
    // allocate memory for the map structure
    map_t *map = malloc(sizeof(map_t));
    if (map == NULL) {
        perror("malloc");
        exit(1);
    }
    // initialize the buckets and locks arrays
    for (int i = 0; i < BUCKEDTS_SIZE; i++) {
        map->buckets[i] = NULL;
        pthread_mutex_init(&map->locks[i], NULL);
    }
    return map;
}

// A function to destroy a hash map
void destroy_map(map_t *map) {
    if (map == NULL) return;
    // free each bucket and its entries
    for (int i = 0; i < BUCKEDTS_SIZE; i++) {
        entry_t *entry = map->buckets[i];
        while (entry != NULL) {
            entry_t *next = entry->next;
            free(entry);
            entry = next;
        }
        pthread_mutex_destroy(&map->locks[i]);
    }
    // free the map structure
    free(map);
}

hook_entry *get(map_t *map, pthread_mutex_t *key) {
    if (map == NULL) return NULL;
    // 转换为质数
    uint64_t index = hash((uint64_t) key);
    // lock the bucket
    pthread_mutex_lock(&map->locks[index]);
    // search for the key in the bucket
    entry_t *entry = map->buckets[index];
    while (entry != NULL) {
        if (entry->key == key) {
            // key found, get the value and unlock the bucket
            hook_entry *value = entry->value;
            pthread_mutex_unlock(&map->locks[index]);
            return value;
        }
        entry = entry->next;
    }
    // key not found, unlock the bucket and return -1
    pthread_mutex_unlock(&map->locks[index]);
    return NULL;
}

// A function to put a key-value pair in the map
// If the key already exists, updates the value
// Returns 0 on success, -1 on failure
int put(map_t *map, pthread_mutex_t *key, hook_entry *value) {
    if (map == NULL) return -1;
    // calculate hash
    uint64_t index = hash((uint64_t) key);
    // lock the bucket
    pthread_mutex_lock(&map->locks[index]);
    // search for the key in the bucket
    entry_t *entry = map->buckets[index];
    while (entry != NULL) {
        if (entry->key == key) {
            // key found, update the value and unlock the bucket
            entry->value = value;
            pthread_mutex_unlock(&map->locks[index]);
            return 0;
        }
        entry = entry->next;
    }
    // key not found, create a new entry and insert it at the head of the bucket
    entry = malloc(sizeof(entry_t));
    if (entry == NULL) {
        perror("malloc");
        pthread_mutex_unlock(&map->locks[index]);
        return -1;
    }
    entry->key = key;
    entry->value = value;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    // unlock the bucket and return 0
    pthread_mutex_unlock(&map->locks[index]);
    return 0;
}

