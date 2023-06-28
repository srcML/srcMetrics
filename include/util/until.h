/**
 * @file until.h
 * @brief Defines the until(condition) macro.
 *
 * This macro is a shortcut for while-not branches.
 *
 * @author Yavuz Koroglu
 */
#ifndef UNTIL_H
    #define UNTIL_H

    /**
     * @def until(condition)
     *   A shortcut for while-not branches.
     */
    #define until(condition) while (!(condition))
#endif
