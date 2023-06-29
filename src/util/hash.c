#include "util/hash.h"

/* The famous HASH33 or djb2 algorithm */

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
