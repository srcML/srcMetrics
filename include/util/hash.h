/**
 * @file hash.h
 * @brief Defines hash function(s).
 * @author Yavuz Koroglu
 */
#ifndef HASH_H
    #define HASH_H

    /**
     * @brief Returns the hash value of a string.
     * @param str The string.
     * @return The hash value of the string.
     */
    unsigned long hash_str(char const* const restrict str);
#endif
