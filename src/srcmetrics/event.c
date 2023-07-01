/**
 * @file event.c
 * @brief Implements functions defined in event.h.
 * @author Yavuz Koroglu
 * @see event.h
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/event.h"
#include "srcmetrics/metrics.h"
#include "srcmetrics/options.h"
#include "srcmetrics/elements/function.h"
#include "srcmetrics/elements/unit.h"
#include "srcmetrics/elements/variable.h"
#include "util/chunk.h"
#include "util/streq.h"
#include "util/svmap.h"
#include "util/unless.h"

static char*        rewind_p;
static int          function_read_state = 0;

static size_t       variables_cap       = VAR_COUNT_GUESS;
static size_t       variables_count     = 0;
static Variable*    variables           = NULL;

static size_t       functions_cap       = FN_COUNT_GUESS;
static size_t       functions_count     = 0;
static Function*    functions           = NULL;

static size_t       units_cap           = UNIT_COUNT_GUESS;
static size_t       units_count         = 0;
static Unit*        units;

static Unit*        currentUnit         = NULL;
static Function*    currentFunction     = NULL;

static size_t       functionStack_cap   = FN_COUNT_GUESS;
static size_t       functionStack_size  = 0;
static Function*    functionStack       = NULL;

static SVMap        unitMap             = NOT_AN_SVMAP;
static SVMap        functionMap         = NOT_AN_SVMAP;
static SVMap        variableMap         = NOT_AN_SVMAP;

static void free_eventElements(void) {
    free(variables);
    free(functions);
    free(units);
    free(functionStack);
    free_svmap(unitMap);
    free_svmap(functionMap);
    free_svmap(variableMap);
}

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
    static char const* metrics[]                = METRICS;
    static Event allEventsAtStartDocument[]     = ALL_EVENTS_AT_START_DOCUMENT;
    static Event allEventsAtEndDocument[]       = ALL_EVENTS_AT_END_DOCUMENT;
    static Event allEventsAtStartRoot[]         = ALL_EVENTS_AT_START_ROOT;
    static Event allEventsAtStartUnit[]         = ALL_EVENTS_AT_START_UNIT;
    static Event allEventsAtStartElement[]      = ALL_EVENTS_AT_START_ELEMENT;
    static Event allEventsAtEndRoot[]           = ALL_EVENTS_AT_START_ROOT;
    static Event allEventsAtEndUnit[]           = ALL_EVENTS_AT_START_UNIT;
    static Event allEventsAtEndElement[]        = ALL_EVENTS_AT_START_ELEMENT;
    static Event allEventsAtCharactersRoot[]    = ALL_EVENTS_AT_CHARACTERS_ROOT;
    static Event allEventsAtCharactersUnit[]    = ALL_EVENTS_AT_CHARACTERS_UNIT;
    static Event allEventsAtMetaTag[]           = ALL_EVENTS_AT_META_TAG;
    static Event allEventsAtComment[]           = ALL_EVENTS_AT_COMMENT;
    static Event allEventsAtCDataBlock[]        = ALL_EVENTS_AT_CDATA_BLOCK;
    static Event allEventsAtProcInfo[]          = ALL_EVENTS_AT_PROC_INFO;

    Event* lastEventOfStartDocument             = eventsAtStartDocument;
    Event* lastEventOfEndDocument               = eventsAtEndDocument;
    Event* lastEventOfStartRoot                 = eventsAtStartRoot;
    Event* lastEventOfStartUnit                 = eventsAtStartUnit;
    Event* lastEventOfStartElement              = eventsAtStartElement;
    Event* lastEventOfEndRoot                   = eventsAtEndRoot;
    Event* lastEventOfEndUnit                   = eventsAtEndUnit;
    Event* lastEventOfEndElement                = eventsAtEndElement;
    Event* lastEventOfCharactersRoot            = eventsAtCharactersRoot;
    Event* lastEventOfCharactersUnit            = eventsAtCharactersUnit;
    Event* lastEventOfMetaTag                   = eventsAtMetaTag;
    Event* lastEventOfComment                   = eventsAtComment;
    Event* lastEventOfCDataBlock                = eventsAtCDataBlock;
    Event* lastEventOfProcInfo                  = eventsAtProcInfo;

    char const** metric      = metrics;
    size_t metricId          = 0;
    uint_fast64_t metricMask = 1;
    for (;
        *metric && metricId < METRICS_COUNT_MAX;
        (metric++, metricId++, metricMask <<= 1)
    ) {
        unless (options.enabledMetrics && metricMask) continue;

        *(lastEventOfStartDocument++)   = allEventsAtStartDocument[metricId];
        *(lastEventOfEndDocument++)     = allEventsAtEndDocument[metricId];
        *(lastEventOfStartRoot++)       = allEventsAtStartRoot[metricId];
        *(lastEventOfStartUnit++)       = allEventsAtStartUnit[metricId];
        *(lastEventOfStartElement++)    = allEventsAtStartElement[metricId];
        *(lastEventOfEndRoot++)         = allEventsAtEndRoot[metricId];
        *(lastEventOfEndUnit++)         = allEventsAtEndUnit[metricId];
        *(lastEventOfEndElement++)      = allEventsAtEndElement[metricId];
        *(lastEventOfCharactersRoot++)  = allEventsAtCharactersRoot[metricId];
        *(lastEventOfCharactersUnit++)  = allEventsAtCharactersUnit[metricId];
        *(lastEventOfMetaTag++)         = allEventsAtMetaTag[metricId];
        *(lastEventOfComment++)         = allEventsAtComment[metricId];
        *(lastEventOfCDataBlock++)      = allEventsAtCDataBlock[metricId];
        *(lastEventOfProcInfo++)        = allEventsAtProcInfo[metricId];
    }
    *lastEventOfStartDocument           = NULL;
    *lastEventOfEndDocument             = NULL;
    *lastEventOfStartRoot               = NULL;
    *lastEventOfStartUnit               = NULL;
    *lastEventOfStartElement            = NULL;
    *lastEventOfEndRoot                 = NULL;
    *lastEventOfEndUnit                 = NULL;
    *lastEventOfEndElement              = NULL;
    *lastEventOfCharactersRoot          = NULL;
    *lastEventOfCharactersUnit          = NULL;
    *lastEventOfMetaTag                 = NULL;
    *lastEventOfComment                 = NULL;
    *lastEventOfCDataBlock              = NULL;
    *lastEventOfProcInfo                = NULL;

    unless (
        (variables || (variables = calloc(variables_cap, sizeof(Variable))))                                &&
        (functions || (functions = calloc(functions_cap, sizeof(Function))))                                &&
        (units || (units = calloc(units_cap, sizeof(Unit))))                                                &&
        (isValid_svmap(unitMap) || isValid_svmap(unitMap = constructEmpty_svmap(UNIT_COUNT_GUESS)))         &&
        (isValid_svmap(functionMap) || isValid_svmap(functionMap = constructEmpty_svmap(FN_COUNT_GUESS)))   &&
        (isValid_svmap(variableMap) || isValid_svmap(variableMap = constructEmpty_svmap(VAR_COUNT_GUESS)))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    /* Free all these allocations at the end */
    atexit(free_eventElements);

    rewind_p = strings.end;

    /* Execute all related events */
    for (Event* event = eventsAtStartDocument; *event; event++)
        (*event)(context);
}
static void event_endDocument(struct srcsax_context* context) {
    /*
    currentUnit         = NULL;
    currentFunction     = NULL;
    strings->end        = rewind_p;
    function_read_state = 0;
    variables_count     = 0;
    functions_count     = 0;
    units_count         = 0;
    functionStack_size  = 0;
    empty_svmap(unitMap);
    empty_svmap(functionMap);
    empty_svmap(variableMap);
    */

    /* Execute all related events */
    for (Event* event = eventsAtEndDocument; *event; event++)
        (*event)(context);
}
static void event_startRoot(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri,
    int                             num_namespaces,
    struct srcsax_namespace const*  namespaces,
    int                             num_attributes,
    struct srcsax_attribute const*  attributes
) {
    /* Execute all related events */
    for (Event* event = eventsAtStartRoot; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
}
static void event_startUnit(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri,
    int                             num_namespaces,
    struct srcsax_namespace const*  namespaces,
    int                             num_attributes,
    struct srcsax_attribute const*  attributes
) {
    unless (
        units_count < units_cap ||
        (
            units_count < (units_cap <<= 1) &&
            (units = realloc(units, units_cap * sizeof(Unit)))
        )
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    unless (
        currentFunction == NULL &&
        functionStack_size == 0 &&
        function_read_state == 0
    ) { fputs("PARSE_ERROR\n", stderr); exit(EXIT_FAILURE); }

    currentUnit = NULL;
    for (struct srcsax_attribute const* attribute = attributes + num_attributes - 1; attribute >= attributes; attribute--) {
        unless (str_eq_const(attribute->localname, "filename")) continue;
        *(currentUnit = units + units_count++) = (Unit){ attribute->value };
    }
    unless (currentUnit) { fputs("PARSE_ERROR\n", stderr); exit(EXIT_FAILURE); }

    unless (insert_svmap(&unitMap, localname, (value_t){ .as_pointer = currentUnit }))
        { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    /* Execute all related events */
    for (Event* event = eventsAtStartUnit; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit);
}
static void event_startElement(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri,
    int                             num_namespaces,
    struct srcsax_namespace const*  namespaces,
    int                             num_attributes,
    struct srcsax_attribute const*  attributes
) {
    if (str_eq_const(localname, "function")) {
        unless (
            (
                functions_count < functions_cap ||
                (
                    functions_count < (functions_cap <<= 1) &&
                    (functions = realloc(functions, functions_cap * sizeof(Function)))
                )
            ) && (
                functionStack_size < functionStack_cap ||
                (
                    functionStack_size < (functionStack_cap <<= 1) &&
                    (functionStack = realloc(functionStack, functionStack_cap * sizeof(Function)))
                )
            )
        ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

        *(currentFunction = functions[functions_count++] = functionStack[functionStack_size++]) = (Function){ "Unknown Function", currentUnit->name, NULL };

        function_read_state = 1;
    } else if (function_read_state == 1 && str_eq_const(localname, "type")) {
        function_read_state = 2;
    } else if (function_read_state == 3 && str_eq_const(localname, "name")) {
        function_read_state = 4;
    }

    /* Execute all related events */
    for (Event* event = eventsAtStartElement; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit, currentFunction);
}
static void event_endRoot(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    /* Execute all related events */
    for (Event* event = eventsAtRoot; *event; event++)
        (*event)(context, localname, prefix, uri);
}
static void event_endUnit(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    /* Execute all related events */
    for (Event* event = eventsAtEndUnit; *event; event++)
        (*event)(context, localname, prefix, uri, currentUnit);

    currentUnit = NULL;
}
static void event_endElement(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    if (str_eq_const(localname, "function")) {
        unless (functionStack_size) { fputs("Corrupt Function Stack\n", stderr); exit(EXIT_FAILURE); }

        function_read_state = (functionStack_size ? 6 : 0);
    } else if (function_read_state == 2 && str_eq_const(localname, "type")) {
        function_read_state == 3;
    } else if (function_read_state == 4 && str_eq_const(localname, "name")) {
        function_read_state == 5;
    } else if (function_read_state == 5 && str_eq_const(currentFunction->name, "Unknown Function")) {
        fputs("PARSE_ERROR\n", stderr);
        exit(EXIT_FAILURE);
    }

    /* Execute all related events */
    for (Event* event = eventsAtEndElement; *event; event++)
        (*event)(context, localname, prefix, uri, currentUnit, currentFunction);

    case (function_read_state) {
        case 6:
            function_read_state = 5;
        case 0:
            currentFunction = --functionStack_size ? functionStack[functionStack_size - 1] : NULL;
    }
}
static void event_charactersRoot(struct srcsax_context* context, char const* ch, int len) {
    /* Execute all related events */
    for (Event* event = eventsAtCharactersRoot; *event; event++)
        (*event)(context, ch, len);
}
static void event_charactersUnit(struct srcsax_context* context, char const* ch, int len) {
    if (function_read_state == 4) {
        currentFunction->name = ch;
        unless (
            (currentFunction->designator = add_chunk(&strings, currentFunction->ownerUnit)) &&
            append_chunk(&strings, "::")                                                    &&
            append_chunk(&strings, currentFunction->name)                                   &&
            insert_svmap(&functionMap, currentFunction->designator, (value_t){ .as_pointer = currentFunction }
        ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
    }

    /* Execute all related events */
    for (Event* event = eventsAtCharactersUnit; *event; event++)
        (*event)(context, ch, len);
}
static void event_metaTag(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri,
    int                             num_namespaces,
    struct srcsax_namespace const*  namespaces,
    int                             num_attributes,
    struct srcsax_attribute const*  attributes
) {
    /* Execute all related events */
    for (Event* event = eventsAtMetaTag; *event; event++)
        (*event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
}
static void event_comment(struct srcsax_context* context, char const* value) {
    /* Execute all related events */
    for (Event* event = eventsAtComment; *event; event++)
        (*event)(context, ch, value);
}
static void event_cdataBlock(struct srcsax_context * context, char const* value, int len) {
    /* Execute all related events */
    for (Event* event = eventsAtCDataBlock; *event; event++)
        (*event)(context, value, len);
}
static void event_procInfo(struct srcsax_context* context, char const* target, char const* data) {
    /* Execute all related events */
    for (Event* event = eventsAtProcInfo; *event; event++)
        (*event)(context, target, data);
}

static struct srcsax_handler* events = {
    &event_startDocument, &event_endDocument,
    &event_startRoot, &event_startUnit, &event_startElement,
    &event_endRoot, &event_endUnit, &event_endElement,
    &event_charactersRoot, &event_charactersUnit,
    &event_metaTag, &event_comment, &event_cdataBlock, &event_procInfo
};
struct srcsax_handler* getStaticEventHandler(void) { return events; }
