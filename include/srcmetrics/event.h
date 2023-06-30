#ifndef EVENT_H
    #define EVENT_H
    #include "libsrcsax/srcsax.h"

    typedef void(*Event)(struct srcsax_context, ...);
#endif
