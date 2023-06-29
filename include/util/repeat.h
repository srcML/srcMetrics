/**
 * @file repeat.h
 * @brief Defines the repeat(n) macro.
 *
 * repeat(n) repeats the following code block or statement n times.
 * It uses an internal down counter called _dcntr.
 *
 * @author Yavuz Koroglu
 */
#ifndef REPEAT_H
    #define REPEAT_H

    /**
     * @def repeat(n)
     *   Repeats the following code block or statement n times
     *   using an internal down counter called _dcntr.
     */
    #define repeat(n) for (unsigned long long _dcntr = (unsigned long long)(n); _dcntr--;)
#endif
