/**
 * @file unless.h
 * @brief Defines the unless(condition) macro.
 *
 * This macro defines a shortcut for if-not branches.
 *
 * @author Yavuz Koroglu
 */
#ifndef UNLESS_H
    #define UNLESS_H

    /**
     * @def unless(condition)
     *   A shortcut for if-not branches.
     */
    #define unless(condition) if (!(condition))
#endif
