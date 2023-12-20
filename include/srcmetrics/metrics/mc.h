/**
 * @file mc.h
 * @brief Method Count
 * @author Yavuz Koroglu
 * @see mc.c
 */
#ifndef MC_H
    #define MC_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_mc (struct srcsax_context* context, ...);
    void event_endDocument_mc   (struct srcsax_context* context, ...);
    void event_startUnit_mc     (struct srcsax_context* context, ...);
    void event_endUnit_mc       (struct srcsax_context* context, ...);
    void event_startElement_mc  (struct srcsax_context* context, ...);
    Map const* report_mc        (void);

    #define MC_EVENT_AT_START_DOCUMENT  &event_startDocument_mc
    #define MC_EVENT_AT_END_DOCUMENT    &event_endDocument_mc
    #define MC_EVENT_AT_START_ROOT      NULL
    #define MC_EVENT_AT_START_UNIT      &event_startUnit_mc
    #define MC_EVENT_AT_START_ELEMENT   &event_startElement_mc
    #define MC_EVENT_AT_END_ROOT        NULL
    #define MC_EVENT_AT_END_UNIT        &event_endUnit_mc
    #define MC_EVENT_AT_END_ELEMENT     NULL
    #define MC_EVENT_AT_CHARACTERS_ROOT NULL
    #define MC_EVENT_AT_CHARACTERS_UNIT NULL
    #define MC_EVENT_AT_META_TAG        NULL
    #define MC_EVENT_AT_COMMENT         NULL
    #define MC_EVENT_AT_CDATA_BLOCK     NULL
    #define MC_EVENT_AT_PROC_INFO       NULL
    #define MC_REPORT                   &report_mc
#endif
