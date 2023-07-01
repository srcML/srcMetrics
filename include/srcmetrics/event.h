#ifndef EVENT_H
    #define EVENT_H
    #include "libsrcsax/srcsax.h"

    typedef void(*Event)(struct srcsax_context*, ...);

    struct srcsax_handler* getStaticEventHandler(void);

    #define ALL_EVENTS_AT_START_DOCUMENT {  \
        NPM_EVENT_AT_START_DOCUMENT,        \
        SLOC_EVENT_AT_START_DOCUMENT,       \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_DOCUMENT {    \
        NPM_EVENT_AT_END_DOCUMENT,          \
        SLOC_EVENT_AT_END_DOCUMENT,         \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_ROOT {      \
        NPM_EVENT_AT_START_ROOT,            \
        SLOC_EVENT_AT_START_ROOT,           \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_UNIT {      \
        NPM_EVENT_AT_START_UNIT,            \
        SLOC_EVENT_AT_START_UNIT,           \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_ELEMENT {   \
        NPM_EVENT_AT_START_ELEMENT,         \
        SLOC_EVENT_AT_START_ELEMENT,        \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_ROOT {        \
        NPM_EVENT_AT_END_ROOT,              \
        SLOC_EVENT_AT_END_ROOT,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_UNIT {        \
        NPM_EVENT_AT_END_UNIT,              \
        SLOC_EVENT_AT_END_UNIT,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_ELEMENT {     \
        NPM_EVENT_AT_END_ELEMENT,           \
        SLOC_EVENT_AT_END_ELEMENT,          \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CHARACTERS_ROOT { \
        NPM_EVENT_AT_CHARACTERS_ROOT,       \
        SLOC_EVENT_AT_CHARACTERS_ROOT,      \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CHARACTERS_UNIT { \
        NPM_EVENT_AT_CHARACTERS_UNIT,       \
        SLOC_EVENT_AT_CHARACTERS_UNIT,      \
        NULL                                \
    }
    #define ALL_EVENTS_AT_META_TAG {        \
        NPM_EVENT_AT_META_TAG,              \
        SLOC_EVENT_AT_META_TAG,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_COMMENT {         \
        NPM_EVENT_AT_COMMENT,               \
        SLOC_EVENT_AT_COMMENT,              \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CDATA_BLOCK {     \
        NPM_EVENT_AT_CDATA_BLOCK,           \
        SLOC_EVENT_AT_CDATA_BLOCK,          \
        NULL                                \
    }
    #define ALL_EVENTS_AT_PROC_INFO {       \
        NPM_EVENT_AT_PROC_INFO,             \
        SLOC_EVENT_AT_PROC_INFO,            \
        NULL                                \
    }
#endif
