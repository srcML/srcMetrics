/**
 * @file sloc.h
 * @brief Source Lines of Code
 * @author Yavuz Koroglu
 * @see sloc.c
 */
#ifndef SLOC_H
    #define SLOC_H
    #include "libsrcsax/srcsax.h"

    void event_startElement_sloc(struct srcsax_context* context, ...);
#endif
