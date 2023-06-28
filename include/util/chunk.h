/**
 * @file chunk.h
 * @brief Defines Chunk and its related functions.
 *
 * A Chunk is a chunk of heap memory. You can add several strings to a Chunk.
 * You can also append strings to the last string of the Chunk.
 * Freeing the Chunk frees all the allocated strings.
 * Modifying/expanding/removing strings from the middle of the Chunk may break the Chunk.
 *
 * @author Yavuz Koroglu
 */
#ifndef CHUNK_H
    #define CHUNK_H
    #include <stddef.h>

    /**
     * @struct Chunk
     * A chunk of heap memory.
     *
     * @var Chunk::cap
     *   The Chunk capacity.
     * @var Chunk::start
     *   A pointer to the beginning of the Chunk
     * @var Chunk::end
     *   A pointer to the end of the Chunk
     */
    typedef struct ChunkBody {
        size_t cap;
        char* start;
        char* end;
    } Chunk;

    /**
     * @brief Constructs an empty Chunk and returns it.
     * @param initial_cap The initial capacity of the Chunk.
     * @return Chunk
     */
    Chunk constructEmpty_chunk(size_t const initial_cap);

    /**
     * @brief Adds a string to a Chunk.
     *
     * Copies n characters from the string to a
     * new string located at the end of the Chunk.
     * The new string is '\0' separated from the
     * previous string.
     *
     * @param chunk The Chunk.
     * @param str The string.
     * @param n #characters to copy.
     * @return A pointer to the string's location in the Chunk.
     */
    char* add_chunk(Chunk* const restrict chunk, char const* const restrict str, size_t const n);

    /**
     * @brief Appends a string to the final string of a Chunk.
     *
     * This function is similar to add_chunk() but
     * does NOT respect the '\0' separator.
     *
     * @param chunk The Chunk.
     * @param str The string.
     * @param n #characters to copy.
     * @return A pointer to the final string in the Chunk.
     */
    char* append_chunk(Chunk* const restrict chunk, char const* const restrict str, size_t const n);

    /**
     * @brief Frees all the strings in a Chunk.
     * @param chunk The Chunk.
     */
    void free_chunk(Chunk* const restrict chunk);
#endif
