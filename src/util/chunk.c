/**
 * @file chunk.c
 * @brief Implements the functions defined in chunk.h
 *
 * This is the fastest possible Chunk implementation.
 * This implementation disregards even the simplest
 * NULL pointer checks, so read Fault Cases, carefully.
 *
 * @see Chunk
 * @author Yavuz Koroglu
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util/chunk.h"
#include "util/unless.h"
#include "util/until.h"

/* Fault Case Analysis
 * ===================
 * initial_cap == 0
 */
Chunk constructEmpty_chunk(size_t const initial_cap) {
    Chunk chunk;
    chunk.cap = initial_cap;
    unless ((chunk.end = chunk.start = malloc(initial_cap))) return NOT_A_CHUNK;
    *(chunk.end) = '\0';
    return chunk;
}

/* Fault Case Analysis
 * ===================
 * chunk is invalid_ptr (e.g. NULL)
 * append_chunk() fails
 */
char* add_chunk(Chunk* const restrict chunk, char const* const restrict str, size_t const n) {
    chunk->end += (chunk->end != chunk->start);
    return append_chunk(chunk, str, n);
}

/* Fault Case Analysis
 * ===================
 * chunk is invalid_ptr (e.g. NULL)
 * str is invalid_ptr (e.g. NULL)
 * chunk->start is invalid_ptr (e.g. NULL)
 * chunk->end is invalid_ptr (e.g. NULL)
 * chunk->end < chunk->start
 * chunk->cap == 0
 * chunk->cap overflows
 * realloc() fails
 */
char* append_chunk(Chunk* const restrict chunk, char const* const restrict str, size_t const n) {
    char* start;
    size_t const len = (size_t)((uintptr_t)chunk->end - (uintptr_t)chunk->start + (uintptr_t)n);
    until (chunk->cap > len) chunk->end = (chunk->start = realloc(chunk->start, (chunk->cap <<= 1))) + len - n;
    *(chunk->end = (start = memcpy(chunk->end, str, n)) + n) = '\0';
    return start;
}

/* Fault Case Analysis
 * ===================
 * free() fails
 */
void free_chunk(Chunk const chunk) {
    free(chunk.start);
}

/* Fault Case Analysis
 * ===================
 */
bool isValid_chunk(Chunk const chunk) {
    return chunk.start && chunk.end && chunk.end >= chunk.start && chunk.cap > (size_t)(chunk.end - chunk.start);
}
