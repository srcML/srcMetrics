/**
 * @file readfile.h
 * @brief Defines functions related to reading a FILE into a Chunk.
 * @see Chunk
 */
#ifndef READFILE_H
    #define READFILE_H
    #include <stdio.h>
    #include "chunk.h"

    /**
     * @brief Reads a FILE to an existing Chunk.
     * @param chunk The Chunk.
     * @param file The FILE.
     * @return NOT_A_CHUNK if something goes wrong, the Chunk otherwise.
     */
    Chunk readChunk(Chunk chunk, FILE* file);

    /**
     * @brief Reads a FILE to a new Chunk.
     * @param file The FILE.
     * @return NOT_A_CHUNK if something goes wrong, the Chunk otherwise.
     */
    Chunk readNewChunk(FILE* file);

    /**
     * @brief Opens a FILE, reads to an existing Chunk, and closes the FILE.
     * @param chunk The Chunk.
     * @param filename The file name.
     * @return NOT_A_CHUNK if something goes wrong, the Chunk otherwise.
     */
    Chunk open_readChunk_close(Chunk chunk, char const* const restrict filename);

    /**
     * @brief Opens a FILE, reads to a new Chunk, and closes the FILE.
     * @param filename The file name.
     * @return NOT_A_CHUNK if something goes wrong, the Chunk otherwise.
     */
    Chunk open_readNewChunk_close(char const* const restrict filename);
#endif
