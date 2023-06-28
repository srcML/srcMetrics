/**
 * @file readfile.h
 */
#ifndef READFILE_H
    #define READFILE_H
    #include <stdio.h>
    #include <stdlib.h>
    #include "chunk.h"

    char* readWholeFile_and_appendString(char* strStart, char* strEnd, FILE* file);
    char* open_readWholeFile_and_appendString_close(char* strStart, char* strEnd, char const* const restrict filename);

    char* readWholeFile_to_newString(FILE* file);
    char* open_readWholeFile_to_newString_close(char const* const restrict filename);

    Chunk readLineByLine_to_chunk(Chunk chunk, FILE* file);
    Chunk readLineByLine_to_newChunk(FILE* file);

    Chunk open_readLineByLine_to_chunk_close(Chunk chunk, char const* const restrict filename);
    Chunk open_readLineByLine_to_newChunk_close(char const* const restrict filename);
#endif
