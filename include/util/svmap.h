#ifndef SVMAP_H
    #define SVMAP_H
    #include <stddef.h>

    typedef union value_t_body {
        char*       as_string;
        void*       as_pointer;
        unsigned    as_unsigned;
    } value_t;

    typedef struct SVPairBody {
        char const*         key;
        value_t             value;
        struct SVPairBody*  next;
    } SVPair;

    typedef struct SVMapBody {
        size_t      size;
        size_t      cap;
        size_t      nRows;
        SVPair*     pairs;
        SVPair**    table;
    } SVMap;

    SVMap constructEmpty_svmap(size_t const);
    SVPair* insert_svmap(SVMap* const restrict, char const* const restrict, value_t const);
    value_t* get_svmap(SVMap const* const restrict, char const* const restrict);
    void deleteKey_svmap(SVMap* const restrict, char const* const restrict);
    void free_svmap(SVMap* const restrict);
#endif
