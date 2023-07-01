/**
 * @file svmap.c
 * @brief Implements the functions defined in svmap.h
 * @see SVMap
 * @author Yavuz Koroglu
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "util/hash.h"
#include "util/svmap.h"
#include "util/unless.h"
#include "util/until.h"

/* Fault Case Analysis
 * ===================
 * odd_x is even
 * odd_x == 1
 */
/**
 * @brief Checks if a given odd number is prime.
 * @param odd_x An odd number
 * @returns 1 if odd_x is prime, 0 otherwise.
 */
static bool isPrime(size_t const odd_x) {
    register size_t n;
    /* unless (odd_x % 2 || odd_x == 1) return 0; */
    for (n = odd_x; n > odd_x / n; n >>= 1);
    while (n * n < odd_x) n++;
    for (n -= !(n % 2); n > 2; n -= 2)
        unless (odd_x % n) return 0;
    return 1;
}

/* Fault Case Analysis
 * ===================
 * initial_cap == 0
 * malloc() fails
 * calloc() fails
 */
SVMap constructEmpty_svmap(size_t const initial_cap) {
    size_t row_count = 4U * (initial_cap / 3U);
    if (row_count < initial_cap) row_count = initial_cap;
    row_count += !(row_count % 2);
    if (row_count < 11U) row_count = 11U;
    until (isPrime(row_count)) {
        row_count += 2;
        unless (row_count < initial_cap) continue;
        row_count -= 2;
        break;
    }
    return (SVMap) {
        0,
        initial_cap,
        row_count,
        malloc(initial_cap * sizeof(SVPair)),
        calloc(row_count, sizeof(SVPair*))
    };
}

/* Fault Case Analysis
 * ===================
 * key is invalid_ptr (e.g. NULL)
 * key misses EOS ('\0')
 * map is invalid_ptr (e.g. NULL)
 * map->nRows <= 1
 * map->table is invalid_ptr (e.g. NULL)
 * map->pairs is invalid_ptr (e.g. NULL)
 */
SVPair* insert_svmap(SVMap* const restrict map, char const* const restrict key, value_t const value) {
    SVPair *prev, *cur;
    size_t const row_id = hash_str(key) % map->nRows;

    unless (map->table[row_id]) {
        unless (map->size < map->cap) {
            SVPair* newPairs;
            unless (
                map->size < (map->cap <<= 1) &&
                (newPairs = realloc(map->pairs, map->cap * sizeof(SVPair)))
            ) return NULL;
            map->pairs = newPairs;
        }
        map->table[row_id] = map->pairs + map->size++;
        map->table[row_id]->key = key;
        map->table[row_id]->value = value;
        map->table[row_id]->next = NULL;
        return map->table[row_id];
    }
    for (prev = NULL, cur = map->table[row_id]; cur; prev = cur, cur = cur->next) {
        if (strcmp(key, cur->key)) continue;
        cur->value = value;
        return cur;
    }
    unless (map->size < map->cap) {
        SVPair* newPairs;
        unless (
            map->size < (map->cap <<= 1) &&
            (newPairs = realloc(map->pairs, map->cap * sizeof(SVPair)))
        ) return NULL
        map->pairs = newPairs;
    }
    prev->next = map->pairs + map->size++;
    prev->next->key = key;
    prev->next->value = value;
    prev->next->next = NULL;
    return prev->next;
}

/* Fault Case Analysis
 * ===================
 * key is invalid_ptr (e.g. NULL)
 * key misses EOS ('\0')
 * map is invalid_ptr (e.g. NULL)
 * map->nRows <= 1
 * map->table is invalid_ptr (e.g. NULL)
 */
value_t* get_svmap(SVMap const* const restrict map, char const* const restrict key) {
    size_t const row_id = hash_str(key) % map->nRows;

    unless (map->table[row_id]) return NULL;
    for (SVPair* cur = map->table[row_id]; cur; cur = cur->next) {
        if (!strcmp(key, cur->key)) return &(cur->value);
    }
    return NULL;
}

/* Fault Case Analysis
 * ===================
 * key is invalid_ptr (e.g. NULL)
 * key misses EOS ('\0')
 * map is invalid_ptr (e.g. NULL)
 * map->nRows <= 1
 * map->table is invalid_ptr (e.g. NULL)
 * map->pairs is invalid_ptr (e.g. NULL)
 * map->size == 0
 * map->size > map->cap
 */
void deleteKey_svmap(SVMap* const restrict map, char const* const restrict key) {
    size_t const row_id = hash_str(key) % map->nRows;

    unless (map->table[row_id]) return;
    for (SVPair *prev = NULL, *cur = map->table[row_id]; cur; prev = cur, cur = cur->next) {
        if (strcmp(key, cur->key)) continue;
        if (prev)
            prev->next = cur->next;
        else
            map->table[row_id] = cur->next;
        for (SVPair* pair = map->pairs + --map->size; pair > cur; pair--) pair[-1] = pair[0];
        return;
    }
}

/* Fault Case Analysis
 * ===================
 * free() fails
 */
void free_svmap(SVMap const map) {
    free(map.pairs);
    free(map.table);
}

/* Fault Case Analysis
 * ===================
 */
bool isValid_svmap(SVMap const map) {
    return map.pairs && map.table && map.nRows > 1 && map.size < map.cap;
}

/*
 * Fault Case Analysis
 * ===================
 * map is invalid_ptr (e.g. NULL)
 * map->nRows <= 1
 * map->table is invalid_ptr (e.g. NULL)
 */
void empty_svmap(SVMap* const restrict map) {
    map->size = 0;
    for (SVPair** pair = map->table + map->nRows - 1; pair >= map->table; pair--)
        *pair = NULL;
}
