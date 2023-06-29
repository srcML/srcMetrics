/**
 * @file readfile.h
 */
#ifndef READFILE_H
    #define READFILE_H
    #include <stdio.h>
    #include "chunk.h"

    Chunk readChunk(Chunk chunk, FILE* file);
    Chunk readNewChunk(FILE* file);
    Chunk open_readChunk_close(Chunk chunk, char const* const restrict filename);
    Chunk open_readNewChunk_close(char const* const restrict filename);
#endif
