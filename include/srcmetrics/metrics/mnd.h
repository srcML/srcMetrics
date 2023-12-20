/**
 * @file mnd.h
 * @brief Maximum Nesting Depth
 * @author Yavuz Koroglu
 * @see mnd.c
 */
#ifndef MND_H
    #define MND_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_mnd (struct srcsax_context* context, ...);
    void event_endDocument_mnd   (struct srcsax_context* context, ...);
    void event_startUnit_mnd     (struct srcsax_context* context, ...);
    void event_endUnit_mnd       (struct srcsax_context* context, ...);
    void event_startElement_mnd  (struct srcsax_context* context, ...);
    void event_endElement_mnd    (struct srcsax_context* context, ...);
    Map const* report_mnd        (void);

    #define MND_EVENT_AT_START_DOCUMENT  &event_startDocument_mnd
    #define MND_EVENT_AT_END_DOCUMENT    &event_endDocument_mnd
    #define MND_EVENT_AT_START_ROOT      NULL
    #define MND_EVENT_AT_START_UNIT      &event_startUnit_mnd
    #define MND_EVENT_AT_START_ELEMENT   &event_startElement_mnd
    #define MND_EVENT_AT_END_ROOT        NULL
    #define MND_EVENT_AT_END_UNIT        &event_endUnit_mnd
    #define MND_EVENT_AT_END_ELEMENT     &event_endElement_mnd
    #define MND_EVENT_AT_CHARACTERS_ROOT NULL
    #define MND_EVENT_AT_CHARACTERS_UNIT NULL
    #define MND_EVENT_AT_META_TAG        NULL
    #define MND_EVENT_AT_COMMENT         NULL
    #define MND_EVENT_AT_CDATA_BLOCK     NULL
    #define MND_EVENT_AT_PROC_INFO       NULL
    #define MND_REPORT                   &report_mnd
#endif
