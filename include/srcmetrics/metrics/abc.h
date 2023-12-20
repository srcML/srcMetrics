/**
 * @file abc.h
 * @brief Assignments, Branches, Conditionals
 * @author Yavuz Koroglu
 * @see abc.c
 */
#ifndef ABC_H
    #define ABC_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_abc  (struct srcsax_context* context, ...);
    void event_endDocument_abc    (struct srcsax_context* context, ...);
    void event_startUnit_abc      (struct srcsax_context* context, ...);
    void event_endUnit_abc        (struct srcsax_context* context, ...);
    void event_startElement_abc   (struct srcsax_context* context, ...);
    void event_endElement_abc     (struct srcsax_context* context, ...);
    void event_charactersUnit_abc (struct srcsax_context* context, ...);
    Map const* report_abc         (void);

    #define ABC_EVENT_AT_START_DOCUMENT  &event_startDocument_abc
    #define ABC_EVENT_AT_END_DOCUMENT    &event_endDocument_abc
    #define ABC_EVENT_AT_START_ROOT      NULL
    #define ABC_EVENT_AT_START_UNIT      &event_startUnit_abc
    #define ABC_EVENT_AT_START_ELEMENT   &event_startElement_abc
    #define ABC_EVENT_AT_END_ROOT        NULL
    #define ABC_EVENT_AT_END_UNIT        &event_endUnit_abc
    #define ABC_EVENT_AT_END_ELEMENT     &event_endElement_abc
    #define ABC_EVENT_AT_CHARACTERS_ROOT NULL
    #define ABC_EVENT_AT_CHARACTERS_UNIT &event_charactersUnit_abc
    #define ABC_EVENT_AT_META_TAG        NULL
    #define ABC_EVENT_AT_COMMENT         NULL
    #define ABC_EVENT_AT_CDATA_BLOCK     NULL
    #define ABC_EVENT_AT_PROC_INFO       NULL
    #define ABC_REPORT                   &report_abc
#endif
