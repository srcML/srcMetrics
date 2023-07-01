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
    #include <stdbool.h>
    #include <stddef.h>

    /**
     * @def NOT_A_CHUNK
     *   A special Chunk denoting a NOT-Chunk. This Chunk cannot pass the isValid_chunk() test.
     */
    #define NOT_A_CHUNK ((Chunk){0, NULL, NULL})

    /**
     * @struct Chunk
     * @brief A chunk of heap memory.
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
     * @brief Constructs an empty Chunk.
     * @param initial_cap The initial capacity of the Chunk.
     * @return Chunk.
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
     * @return NULL if something goes wrong, a pointer to the string's location in the Chunk otherwise.
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
     * @return NULL if something goes wrong, a pointer to the string's location in the Chunk otherwise.
     */
    char* append_chunk(Chunk* const restrict chunk, char const* const restrict str, size_t const n);

    /**
     * @brief Frees all the strings in a Chunk.
     * @param chunk The Chunk.
     */
    void free_chunk(Chunk const chunk);

    /**
     * @brief Checks if a Chunk is valid.
     * @param chunk The Chunk.
     * @return 1 if the Chunk is valid, 0 otherwise.
     */
    bool isValid_chunk(Chunk const chunk);
#endif
