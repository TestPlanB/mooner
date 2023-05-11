//
// Created by chenhailiang on 2023/7/8.
//

#ifndef DRIVERHOOK_CONCURRENT_HASH_MAP_GAP_H
#define DRIVERHOOK_CONCURRENT_HASH_MAP_GAP_H
#define BUCKEDTS_SIZE 511 // number of buckets in the map
#define MAX_KEY_LEN 32 // maximum length of keys
typedef struct {
    char * cfi_backtrace;
    void *addr;
} hook_entry;

typedef struct entry {
    pthread_mutex_t* key; // the key
    hook_entry *value; // the value
    struct entry *next; // pointer to the next entry in the same bucket
} entry_t;

// A structure to represent a hash map
typedef struct map {
    entry_t *buckets[BUCKEDTS_SIZE]; // an array of pointers to hash buckets
    pthread_mutex_t locks[BUCKEDTS_SIZE]; // an array of mutexes for each bucket
} map_t;


map_t *create_map();

int put(map_t *map,  pthread_mutex_t* key, hook_entry *value);

hook_entry *get(map_t *map,  pthread_mutex_t* key);



#endif //DRIVERHOOK_CONCURRENT_HASH_MAP_GAP_H
