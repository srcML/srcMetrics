#ifndef CHUNK_H
    #define CHUNK_H
    #include <stddef.h>

    typedef struct ChunkBody {
        size_t cap;
        char* start;
        char* end;
    } Chunk;

    Chunk constructEmpty_chunk(size_t const initial_cap);
    char* add_chunk(Chunk* const restrict, char const* const restrict, size_t const);
    char* append_chunk(Chunk* const restrict, char const* const restrict, size_t const);
    void free_chunk(Chunk* const restrict chunk);
#endif
