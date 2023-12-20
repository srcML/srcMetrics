/**
 * @file ams.h
 * @brief Assignments, Branches, Conditionals
 * @author Yavuz Koroglu
 * @see ams.c
 */
#ifndef AMS_H
    #define AMS_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_ams (struct srcsax_context* context, ...);
    void event_endDocument_ams   (struct srcsax_context* context, ...);
    void event_startUnit_ams     (struct srcsax_context* context, ...);
    void event_endUnit_ams       (struct srcsax_context* context, ...);
    void event_startElement_ams  (struct srcsax_context* context, ...);
    void event_endElement_ams    (struct srcsax_context* context, ...);
    Map const* report_ams        (void);

    #define AMS_EVENT_AT_START_DOCUMENT  &event_startDocument_ams
    #define AMS_EVENT_AT_END_DOCUMENT    &event_endDocument_ams
    #define AMS_EVENT_AT_START_ROOT      NULL
    #define AMS_EVENT_AT_START_UNIT      &event_startUnit_ams
    #define AMS_EVENT_AT_START_ELEMENT   &event_startElement_ams
    #define AMS_EVENT_AT_END_ROOT        NULL
    #define AMS_EVENT_AT_END_UNIT        &event_endUnit_ams
    #define AMS_EVENT_AT_END_ELEMENT     &event_endElement_ams
    #define AMS_EVENT_AT_CHARACTERS_ROOT NULL
    #define AMS_EVENT_AT_CHARACTERS_UNIT NULL
    #define AMS_EVENT_AT_META_TAG        NULL
    #define AMS_EVENT_AT_COMMENT         NULL
    #define AMS_EVENT_AT_CDATA_BLOCK     NULL
    #define AMS_EVENT_AT_PROC_INFO       NULL
    #define AMS_REPORT                   &report_ams
#endif
