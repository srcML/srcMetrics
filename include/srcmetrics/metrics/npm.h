#ifndef NPM_H
    #define NPM_H
    #include <stddef.h>
    #include "libsrcsax/srcsax.h"

    void event_startUnit_npm        (struct srcsax_context* context, ...);
    void event_endUnit_npm          (struct srcsax_context* context, ...);
    void event_startElement_npm     (struct srcsax_context* context, ...);
    void event_endElement_npm       (struct srcsax_context* context, ...);
    void event_charactersUnit_npm   (struct srcsax_context* context, ...);
    void report_npm                 (void);

    #define NPM_EVENT_AT_START_DOCUMENT     NULL
    #define NPM_EVENT_AT_END_DOCUMENT       NULL
    #define NPM_EVENT_AT_START_ROOT         NULL
    #define NPM_EVENT_AT_START_UNIT         event_startUnit_npm
    #define NPM_EVENT_AT_START_ELEMENT      event_startElement_npm
    #define NPM_EVENT_AT_END_ROOT           NULL
    #define NPM_EVENT_AT_END_UNIT           event_endUnit_npm
    #define NPM_EVENT_AT_END_ELEMENT        event_endElement_npm
    #define NPM_EVENT_AT_CHARACTERS_ROOT    NULL
    #define NPM_EVENT_AT_CHARACTERS_UNIT    event_charactersUnit_npm
    #define NPM_EVENT_AT_META_TAG           NULL
    #define NPM_EVENT_AT_COMMENT            NULL
    #define NPM_EVENT_AT_CDATA_BLOCK        NULL
    #define NPM_EVENT_AT_PROC_INFO          NULL
    #define NPM_REPORT                      report_npm
#endif
