/**
 * @file hsm.h
 * @brief Halstead Software Metrics
 * @author Yavuz Koroglu
 * @see hsm.c
 */
#ifndef HSM_H
    #define HSM_H
    #include "libsrcsax/srcsax.h"
    #include "padkit/map.h"

    void event_startDocument_hsm  (struct srcsax_context* context, ...);
    void event_endDocument_hsm    (struct srcsax_context* context, ...);
    void event_startUnit_hsm      (struct srcsax_context* context, ...);
    void event_endUnit_hsm        (struct srcsax_context* context, ...);
    void event_startElement_hsm   (struct srcsax_context* context, ...);
    void event_endElement_hsm     (struct srcsax_context* context, ...);
    void event_charactersUnit_hsm (struct srcsax_context* context, ...);
    Map const* report_hsm         (void);

    #define HSM_EVENT_AT_START_DOCUMENT  &event_startDocument_hsm
    #define HSM_EVENT_AT_END_DOCUMENT    &event_endDocument_hsm
    #define HSM_EVENT_AT_START_ROOT      NULL
    #define HSM_EVENT_AT_START_UNIT      &event_startUnit_hsm
    #define HSM_EVENT_AT_START_ELEMENT   &event_startElement_hsm
    #define HSM_EVENT_AT_END_ROOT        NULL
    #define HSM_EVENT_AT_END_UNIT        &event_endUnit_hsm
    #define HSM_EVENT_AT_END_ELEMENT     &event_endElement_hsm
    #define HSM_EVENT_AT_CHARACTERS_ROOT NULL
    #define HSM_EVENT_AT_CHARACTERS_UNIT &event_charactersUnit_hsm
    #define HSM_EVENT_AT_META_TAG        NULL
    #define HSM_EVENT_AT_COMMENT         NULL
    #define HSM_EVENT_AT_CDATA_BLOCK     NULL
    #define HSM_EVENT_AT_PROC_INFO       NULL
    #define HSM_REPORT                   &report_hsm
#endif
