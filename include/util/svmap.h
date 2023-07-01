/**
 * @file svmap.h
 * @brief Defines SVMap (String-Value Map), SVPair (String-Value Pair), and value_t.
 * @author Yavuz Koroglu
 */
#ifndef SVMAP_H
    #define SVMAP_H
    #include <stdbool.h>
    #include <stddef.h>

    /**
     * @def NOT_AN_SVMAP
     *   A special SVMap denoting a NOT-SVMap. This SVMap cannot pass the isValid_svmap() test.
     */
    #define NOT_AN_SVMAP ((SVMap){ 0, 0, 0, NULL, NULL })

    typedef union value_t_body {
        char*       as_string;
        void*       as_pointer;
        double      as_double;
        long        as_long;
        unsigned    as_unsigned;
    } value_t;

    /**
     * @struct SVPair
     * @brief A key-value pair in the form of string-value_t.
     *
     * @var SVPair::key
     *   The key string.
     * @var SVPair::value
     *   The value.
     * @var SVPair::next
     *   Next pair in case of a hash-collision, NULL otherwise.
     */
    typedef struct SVPairBody {
        char const*         key;
        value_t             value;
        struct SVPairBody*  next;
    } SVPair;

    /**
     * @struct SVMap
     * @brief A String-Value Map, based on a hash table.
     *
     * @var SVMap::size
     *   #elements in the table.
     * @var SVMap::cap
     *   The capacity
     * @var SVMap::nRows
     *   #rows in the table.
     * @var SVMap::pairs
     *   The array of pairs.
     * @var SVMap::table
     *   The hash table
     */
    typedef struct SVMapBody {
        size_t      size;
        size_t      cap;
        size_t      nRows;
        SVPair*     pairs;
        SVPair**    table;
    } SVMap;

    /**
     * @brief Constructs an empty SVMap.
     *
     * WARNING: The initial capacity cannot be any positive integer.
     * This function will use the closest suitable integer.
     *
     * @param initial_cap The initial capacity of the SVMap.
     * @return SVMap.
     */
    SVMap constructEmpty_svmap(size_t const initial_cap);

    /**
     * @brief Inserts a SVPair to an SVMap.
     *
     * If the key already is in the SVMap, the previous value is overwritten.
     *
     * @param map The SVMap.
     * @param key The key string.
     * @param value The value.
     * @return NULL if insertion went wrong, a pointer to the inserted SVPair otherwise.
     */
    SVPair* insert_svmap(SVMap* const restrict map, char const* const restrict key, value_t const value);

    /**
     * @brief Gets a value from an SVMap.
     * @param map The SVMap.
     * @param key The key string.
     * @return NULL if the key is NOT found, a pointer to the corresponding value otherwise.
     */
    value_t* get_svmap(SVMap const* const restrict map, char const* const restrict key);

    /**
     * @brief Deletes a key-value pair from an SVMap.
     *
     * Does nothing if the key is NOT found.
     * WARNING: Does NOT call free() for either the key or the value.
     *
     * @param map The SVMap.
     * @param key The key string.
     */
    void deleteKey_svmap(SVMap* const restrict map, char const* const restrict key);

    /**
     * @brief Frees an SVMap.
     * @param map The SVMap.
     */
    void free_svmap(SVMap const map);

    /**
     * @brief Checks if an SVMap is valid.
     * @param map The SVMap.
     * @return 1 if the SVMap is valid, 0 otherwise.
     */
    bool isValid_svmap(SVMap const map);
#endif
