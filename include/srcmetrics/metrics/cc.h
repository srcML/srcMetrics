/**
 * @file cc.h
 * @brief Cyclomatic Complexity
 * @author Yavuz Koroglu
 * @see cc.c
 */
#ifndef CC_H
    #define CC_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_cc  (struct srcsax_context* context, ...);
    void event_endDocument_cc    (struct srcsax_context* context, ...);
    void event_startUnit_cc      (struct srcsax_context* context, ...);
    void event_endUnit_cc        (struct srcsax_context* context, ...);
    void event_startElement_cc   (struct srcsax_context* context, ...);
    void event_endElement_cc     (struct srcsax_context* context, ...);
    void event_charactersUnit_cc (struct srcsax_context* context, ...);
    Map const* report_cc         (void);

    #define CC_EVENT_AT_START_DOCUMENT  &event_startDocument_cc
    #define CC_EVENT_AT_END_DOCUMENT    &event_endDocument_cc
    #define CC_EVENT_AT_START_ROOT      NULL
    #define CC_EVENT_AT_START_UNIT      &event_startUnit_cc
    #define CC_EVENT_AT_START_ELEMENT   &event_startElement_cc
    #define CC_EVENT_AT_END_ROOT        NULL
    #define CC_EVENT_AT_END_UNIT        &event_endUnit_cc
    #define CC_EVENT_AT_END_ELEMENT     &event_endElement_cc
    #define CC_EVENT_AT_CHARACTERS_ROOT NULL
    #define CC_EVENT_AT_CHARACTERS_UNIT &event_charactersUnit_cc
    #define CC_EVENT_AT_META_TAG        NULL
    #define CC_EVENT_AT_COMMENT         NULL
    #define CC_EVENT_AT_CDATA_BLOCK     NULL
    #define CC_EVENT_AT_PROC_INFO       NULL
    #define CC_REPORT                   &report_cc
#endif
