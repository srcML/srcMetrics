/**
 * @file streq.h
 * @brief Defines the str_eq_const(str,strconst) macro.
 *
 * This macro compares any string to a string literal or char[].
 * String literals and char arrays are special because
 * the sizeof(literal_or_charArray) macro determines their length
 * in compile-time.
 *
 * @author Yavuz Koroglu
 */
#ifndef STREQ_H
    #define STREQ_H
    #include <string.h>

    /**
     * @def str_eq_const(str,strconst)
     *   Compares any string to a string literal or char[].
     *   String literals and char arrays are special because
     *   the sizeof(literal_or_charArray) macro determines their length
     *   in compile-time.
     */
    #define str_eq_const(str,strconst)   (!strncmp(str,strconst,sizeof(strconst)-1))
#endif
