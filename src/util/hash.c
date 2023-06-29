/**
 * @file hash.c
 * @brief Implements the functions defined in hash.h.
 *
 * The implementation follows the famous HASH33/djb2 algorithm.
 *
 * @see hash.h
 * @author Yavuz Koroglu
 */
#include "util/hash.h"

/* Fault Case Analysis
 * ==============
 * str is invalid_ptr (e.g. NULL)
 * str misses EOS ('\0')
 */
unsigned long hash_str(char const* restrict str) {
    unsigned long hash = 5381UL;
    for (unsigned long c; (c = (unsigned long)*(str++)); hash += (hash << 5) + c);
    return hash;
}
