/**
 * @file sloc.h
 * @brief Source Lines of Code
 * @author Yavuz Koroglu
 * @see sloc.c
 */
#ifndef SLOC_H
    #define SLOC_H
    #include <stddef.h>
    #include "libsrcsax/srcsax.h"
    #include "util/svmap.h"

    void event_startDocument_sloc   (struct srcsax_context* context, ...);
    void event_endDocument_sloc     (struct srcsax_context* context, ...);
    void event_startUnit_sloc       (struct srcsax_context* context, ...);
    void event_endUnit_sloc         (struct srcsax_context* context, ...);
    void event_startElement_sloc    (struct srcsax_context* context, ...);
    void event_endElement_sloc      (struct srcsax_context* context, ...);
    SVMap* report_sloc              (void);

    #define SLOC_EVENT_AT_START_DOCUMENT    event_startDocument_sloc
    #define SLOC_EVENT_AT_END_DOCUMENT      event_endDocument_sloc
    #define SLOC_EVENT_AT_START_ROOT        NULL
    #define SLOC_EVENT_AT_START_UNIT        event_startUnit_sloc
    #define SLOC_EVENT_AT_START_ELEMENT     NULL
    #define SLOC_EVENT_AT_END_ROOT          NULL
    #define SLOC_EVENT_AT_END_UNIT          event_endUnit_sloc
    #define SLOC_EVENT_AT_END_ELEMENT       event_endElement_sloc
    #define SLOC_EVENT_AT_CHARACTERS_ROOT   NULL
    #define SLOC_EVENT_AT_CHARACTERS_UNIT   NULL
    #define SLOC_EVENT_AT_META_TAG          NULL
    #define SLOC_EVENT_AT_COMMENT           NULL
    #define SLOC_EVENT_AT_CDATA_BLOCK       NULL
    #define SLOC_EVENT_AT_PROC_INFO         NULL
    #define SLOC_REPORT                     report_sloc
#endif
