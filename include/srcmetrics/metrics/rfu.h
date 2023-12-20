/**
 * @file rfu.h
 * @brief Response for Unit
 * @author Yavuz Koroglu
 * @see rfu.c
 */
#ifndef RFU_H
    #define RFU_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_rfu  (struct srcsax_context* context, ...);
    void event_endDocument_rfu    (struct srcsax_context* context, ...);
    void event_startUnit_rfu      (struct srcsax_context* context, ...);
    void event_endUnit_rfu        (struct srcsax_context* context, ...);
    void event_startElement_rfu   (struct srcsax_context* context, ...);
    void event_endElement_rfu     (struct srcsax_context* context, ...);
    void event_charactersUnit_rfu (struct srcsax_context* context, ...);
    Map const* report_rfu         (void);

    #define RFU_EVENT_AT_START_DOCUMENT  &event_startDocument_rfu
    #define RFU_EVENT_AT_END_DOCUMENT    &event_endDocument_rfu
    #define RFU_EVENT_AT_START_ROOT      NULL
    #define RFU_EVENT_AT_START_UNIT      &event_startUnit_rfu
    #define RFU_EVENT_AT_START_ELEMENT   &event_startElement_rfu
    #define RFU_EVENT_AT_END_ROOT        NULL
    #define RFU_EVENT_AT_END_UNIT        &event_endUnit_rfu
    #define RFU_EVENT_AT_END_ELEMENT     &event_endElement_rfu
    #define RFU_EVENT_AT_CHARACTERS_ROOT NULL
    #define RFU_EVENT_AT_CHARACTERS_UNIT &event_charactersUnit_rfu
    #define RFU_EVENT_AT_META_TAG        NULL
    #define RFU_EVENT_AT_COMMENT         NULL
    #define RFU_EVENT_AT_CDATA_BLOCK     NULL
    #define RFU_EVENT_AT_PROC_INFO       NULL
    #define RFU_REPORT                   &report_rfu
#endif
