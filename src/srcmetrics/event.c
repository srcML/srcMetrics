/**
 * @file event.c
 * @brief Implements functions defined in event.h.
 * @author Yavuz Koroglu
 * @see event.h
 */
#include "srcmetrics.h"
#include "srcmetrics/event.h"
#include "srcmetrics/metrics.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/map.h"
#include "padkit/streq.h"

static unsigned function_read_state = 0U;

static uint32_t currentUnit_id = 0xFFFFFFFF;
static uint32_t currentFn_id   = 0xFFFFFFFF;

static Event eventsAtStartDocument  [METRICS_COUNT_MAX + 1];
static Event eventsAtEndDocument    [METRICS_COUNT_MAX + 1];
static Event eventsAtStartRoot      [METRICS_COUNT_MAX + 1];
static Event eventsAtStartUnit      [METRICS_COUNT_MAX + 1];
static Event eventsAtStartElement   [METRICS_COUNT_MAX + 1];
static Event eventsAtEndRoot        [METRICS_COUNT_MAX + 1];
static Event eventsAtEndUnit        [METRICS_COUNT_MAX + 1];
static Event eventsAtEndElement     [METRICS_COUNT_MAX + 1];
static Event eventsAtCharactersRoot [METRICS_COUNT_MAX + 1];
static Event eventsAtCharactersUnit [METRICS_COUNT_MAX + 1];
static Event eventsAtMetaTag        [METRICS_COUNT_MAX + 1];
static Event eventsAtComment        [METRICS_COUNT_MAX + 1];
static Event eventsAtCDataBlock     [METRICS_COUNT_MAX + 1];
static Event eventsAtProcInfo       [METRICS_COUNT_MAX + 1];

static void event_startDocument(struct srcsax_context* context) {
    static char const* metrics[]             = METRICS;
    static Event allEventsAtStartDocument[]  = ALL_EVENTS_AT_START_DOCUMENT;
    static Event allEventsAtEndDocument[]    = ALL_EVENTS_AT_END_DOCUMENT;
    static Event allEventsAtStartRoot[]      = ALL_EVENTS_AT_START_ROOT;
    static Event allEventsAtStartUnit[]      = ALL_EVENTS_AT_START_UNIT;
    static Event allEventsAtStartElement[]   = ALL_EVENTS_AT_START_ELEMENT;
    static Event allEventsAtEndRoot[]        = ALL_EVENTS_AT_END_ROOT;
    static Event allEventsAtEndUnit[]        = ALL_EVENTS_AT_END_UNIT;
    static Event allEventsAtEndElement[]     = ALL_EVENTS_AT_END_ELEMENT;
    static Event allEventsAtCharactersRoot[] = ALL_EVENTS_AT_CHARACTERS_ROOT;
    static Event allEventsAtCharactersUnit[] = ALL_EVENTS_AT_CHARACTERS_UNIT;
    static Event allEventsAtMetaTag[]        = ALL_EVENTS_AT_META_TAG;
    static Event allEventsAtComment[]        = ALL_EVENTS_AT_COMMENT;
    static Event allEventsAtCDataBlock[]     = ALL_EVENTS_AT_CDATA_BLOCK;
    static Event allEventsAtProcInfo[]       = ALL_EVENTS_AT_PROC_INFO;

    Event* lastEventOfStartDocument          = eventsAtStartDocument;
    Event* lastEventOfEndDocument            = eventsAtEndDocument;
    Event* lastEventOfStartRoot              = eventsAtStartRoot;
    Event* lastEventOfStartUnit              = eventsAtStartUnit;
    Event* lastEventOfStartElement           = eventsAtStartElement;
    Event* lastEventOfEndRoot                = eventsAtEndRoot;
    Event* lastEventOfEndUnit                = eventsAtEndUnit;
    Event* lastEventOfEndElement             = eventsAtEndElement;
    Event* lastEventOfCharactersRoot         = eventsAtCharactersRoot;
    Event* lastEventOfCharactersUnit         = eventsAtCharactersUnit;
    Event* lastEventOfMetaTag                = eventsAtMetaTag;
    Event* lastEventOfComment                = eventsAtComment;
    Event* lastEventOfCDataBlock             = eventsAtCDataBlock;
    Event* lastEventOfProcInfo               = eventsAtProcInfo;

    char const** metric = metrics;
    size_t metricId     = 0;
    for (uint_fast64_t enabledMetrics = options.enabledMetrics;
        *metric && metricId < METRICS_COUNT_MAX;
        (metric++, metricId++, enabledMetrics >>= 1)
    ) {
        if (!(enabledMetrics & 1)) continue;

        if (allEventsAtStartDocument[metricId])  *(lastEventOfStartDocument++)  = allEventsAtStartDocument[metricId];
        if (allEventsAtEndDocument[metricId])    *(lastEventOfEndDocument++)    = allEventsAtEndDocument[metricId];
        if (allEventsAtStartRoot[metricId])      *(lastEventOfStartRoot++)      = allEventsAtStartRoot[metricId];
        if (allEventsAtStartUnit[metricId])      *(lastEventOfStartUnit++)      = allEventsAtStartUnit[metricId];
        if (allEventsAtStartElement[metricId])   *(lastEventOfStartElement++)   = allEventsAtStartElement[metricId];
        if (allEventsAtEndRoot[metricId])        *(lastEventOfEndRoot++)        = allEventsAtEndRoot[metricId];
        if (allEventsAtEndUnit[metricId])        *(lastEventOfEndUnit++)        = allEventsAtEndUnit[metricId];
        if (allEventsAtEndElement[metricId])     *(lastEventOfEndElement++)     = allEventsAtEndElement[metricId];
        if (allEventsAtCharactersRoot[metricId]) *(lastEventOfCharactersRoot++) = allEventsAtCharactersRoot[metricId];
        if (allEventsAtCharactersUnit[metricId]) *(lastEventOfCharactersUnit++) = allEventsAtCharactersUnit[metricId];
        if (allEventsAtMetaTag[metricId])        *(lastEventOfMetaTag++)        = allEventsAtMetaTag[metricId];
        if (allEventsAtComment[metricId])        *(lastEventOfComment++)        = allEventsAtComment[metricId];
        if (allEventsAtCDataBlock[metricId])     *(lastEventOfCDataBlock++)     = allEventsAtCDataBlock[metricId];
        if (allEventsAtProcInfo[metricId])       *(lastEventOfProcInfo++)       = allEventsAtProcInfo[metricId];
    }
    *lastEventOfStartDocument  = NULL;
    *lastEventOfEndDocument    = NULL;
    *lastEventOfStartRoot      = NULL;
    *lastEventOfStartUnit      = NULL;
    *lastEventOfStartElement   = NULL;
    *lastEventOfEndRoot        = NULL;
    *lastEventOfEndUnit        = NULL;
    *lastEventOfEndElement     = NULL;
    *lastEventOfCharactersRoot = NULL;
    *lastEventOfCharactersUnit = NULL;
    *lastEventOfMetaTag        = NULL;
    *lastEventOfComment        = NULL;
    *lastEventOfCDataBlock     = NULL;
    *lastEventOfProcInfo       = NULL;

    DEBUG_ASSERT(isValid_chunk(strings))

    VERBOSE_MSG_LITERAL("SRCSAX_START => document");

    currentFn_id   = 0xFFFFFFFF;
    currentUnit_id = 0xFFFFFFFF;

    /* Execute all related events */
    for (Event* event = eventsAtStartDocument; *event; event++)
        (*event)(context);
}
static void event_endDocument(struct srcsax_context* context) {
    VERBOSE_MSG_LITERAL("SRCSAX_END => document");

    /* Execute all related events */
    for (Event* event = eventsAtEndDocument; *event; event++)
        (*event)(context);
}
static void event_startRoot(
    struct srcsax_context*         context,
    char const*                    localname,
    char const*                    prefix,
    char const*                    uri,
    int                            num_namespaces,
    struct srcsax_namespace const* namespaces,
    int                            num_attributes,
    struct srcsax_attribute const* attributes
) {
    VERBOSE_MSG_LITERAL("SRCSAX_START => root");

    /* Execute all related events */
    for (Event* event = eventsAtStartRoot; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
}
static void event_startUnit(
    struct srcsax_context*         context,
    char const*                    localname,
    char const*                    prefix,
    char const*                    uri,
    int                            num_namespaces,
    struct srcsax_namespace const* namespaces,
    int                            num_attributes,
    struct srcsax_attribute const* attributes
) {
    DEBUG_ASSERT(function_read_state == 0U)

    for (struct srcsax_attribute const* attribute = attributes + num_attributes - 1; attribute >= attributes; attribute--) {
        if (!STR_EQ_CONST(attribute->localname, "filename")) continue;
        size_t const unit_len = strlen(attribute->value);
        currentUnit_id = add_chunk(strings, attribute->value, unit_len);
        VERBOSE_MSG_VARIADIC("SRCSAX_START => unit (%.*s)", (int)unit_len, attribute->value);
        break;
    }
    DEBUG_ERROR_IF(currentUnit_id == 0xFFFFFFFF)

    /* Execute all related events */
    for (Event* event = eventsAtStartUnit; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit_id);
}
static void event_startElement(
    struct srcsax_context*         context,
    char const*                    localname,
    char const*                    prefix,
    char const*                    uri,
    int                            num_namespaces,
    struct srcsax_namespace const* namespaces,
    int                            num_attributes,
    struct srcsax_attribute const* attributes
) {
    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_LITERAL("SRCSAX_START => function");
        function_read_state = 1U;
        currentFn_id = addIndex_chunk(strings, currentUnit_id);
        DEBUG_ERROR_IF(currentFn_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(append_chunk(strings, "::", 2) == NULL)
        NDEBUG_EXECUTE(append_chunk(strings, "::", 2))
    } else if (function_read_state == 1 && STR_EQ_CONST(localname, "type")) {
        VERBOSE_MSG_LITERAL("SRCSAX_START => function_type");
        function_read_state = 2U;
    } else if (function_read_state == 3 && STR_EQ_CONST(localname, "name")) {
        VERBOSE_MSG_LITERAL("SRCSAX_START => function_name");
        function_read_state = 4U;
    }

    /* Execute all related events */
    for (Event* event = eventsAtStartElement; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit_id, currentFn_id);
}
static void event_endRoot(
    struct srcsax_context* context,
    char const*            localname,
    char const*            prefix,
    char const*            uri
) {
    VERBOSE_MSG_LITERAL("SRCSAX_END => root");

    /* Execute all related events */
    for (Event* event = eventsAtEndRoot; *event; event++)
        (*event)(context, localname, prefix, uri);
}
static void event_endUnit(
    struct srcsax_context* context,
    char const*            localname,
    char const*            prefix,
    char const*            uri
) {
    VERBOSE_MSG_VARIADIC("SRCSAX_END => unit (%s)", get_chunk(strings, currentUnit_id));

    /* Execute all related events */
    for (Event* event = eventsAtEndUnit; *event; event++)
        (*event)(context, localname, prefix, uri, currentUnit_id);

    currentUnit_id = 0xFFFFFFFF;
}
static void event_endElement(
    struct srcsax_context* context,
    char const*            localname,
    char const*            prefix,
    char const*            uri
) {
    bool closeFn = 0;
    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("SRCSAX_END => function (%s)", get_chunk(strings, currentFn_id));
        function_read_state = 0U;
        closeFn             = 1;
    } else if (function_read_state == 2U && STR_EQ_CONST(localname, "type")) {
        VERBOSE_MSG_LITERAL("SRCSAX_END => function_type");
        function_read_state = 3U;
    } else if (function_read_state == 4U && STR_EQ_CONST(localname, "name")) {
        DEBUG_ERROR_IF(append_chunk(strings, "()", 2) == NULL)
        NDEBUG_EXECUTE(append_chunk(strings, "()", 2))
        VERBOSE_MSG_VARIADIC("SRCSAX_END => function_nane (%s)", get_chunk(strings, currentFn_id));
        function_read_state = 5U;
    }
    DEBUG_ERROR_IF(function_read_state == 5U && currentFn_id == 0xFFFFFFFF)

    /* Execute all related events */
    for (Event* event = eventsAtEndElement; *event; event++)
        (*event)(context, localname, prefix, uri, currentUnit_id, currentFn_id);

    if (closeFn) currentFn_id = 0xFFFFFFFF;
    return;
}
static void event_charactersRoot(struct srcsax_context* context, char const* ch, int len) {
    /* Execute all related events */
    for (Event* event = eventsAtCharactersRoot; *event; event++)
        (*event)(context, ch, (uint64_t)len);
}
static void event_charactersUnit(struct srcsax_context* context, char const* ch, int len) {
    if (function_read_state == 4U) {
        DEBUG_ERROR_IF(append_chunk(strings, ch, (uint64_t)len) == NULL)
        NDEBUG_EXECUTE(append_chunk(strings, ch, (uint64_t)len))
    }

    /* Execute all related events */
    for (Event* event = eventsAtCharactersUnit; *event; event++)
        (*event)(context, ch, (uint64_t)len, currentUnit_id, currentFn_id);
}
static void event_metaTag(
    struct srcsax_context*         context,
    char const*                    localname,
    char const*                    prefix,
    char const*                    uri,
    int                            num_namespaces,
    struct srcsax_namespace const* namespaces,
    int                            num_attributes,
    struct srcsax_attribute const* attributes
) {
    /* Execute all related events */
    for (Event* event = eventsAtMetaTag; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
}
static void event_comment(struct srcsax_context* context, char const* value) {
    /* Execute all related events */
    for (Event* event = eventsAtComment; *event; event++)
        (*event)(context, value);
}
static void event_cdataBlock(struct srcsax_context * context, char const* value, int len) {
    /* Execute all related events */
    for (Event* event = eventsAtCDataBlock; *event; event++)
        (*event)(context, value, (uint64_t)len);
}
static void event_procInfo(struct srcsax_context* context, char const* target, char const* data) {
    /* Execute all related events */
    for (Event* event = eventsAtProcInfo; *event; event++)
        (*event)(context, target, data);
}

static struct srcsax_handler events[1] = {{
    &event_startDocument, &event_endDocument,
    &event_startRoot, &event_startUnit, &event_startElement,
    &event_endRoot, &event_endUnit, &event_endElement,
    &event_charactersRoot, &event_charactersUnit,
    &event_metaTag, &event_comment, &event_cdataBlock, &event_procInfo
}};
struct srcsax_handler* getStaticEventHandler(void) { return events; }
