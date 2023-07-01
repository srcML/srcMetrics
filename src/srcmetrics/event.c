/**
 * @file event.c
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/event.h"
#include "srcmetrics/metrics.h"
#include "srcmetrics/elements/function.h"
#include "srcmetrics/elements/unit.h"
#include "srcmetrics/elements/variable.h"
#include "util/chunk.h"
#include "util/svmap.h"

static size_t           variables_cap       = BUFSIZ;
static size_t           variables_count     = 0;
static Variable*        variables;

static size_t           functions_cap       = BUFSIZ;
static size_t           functions_count     = 0;
static Function*        functions;

static size_t           units_cap           = BUFSIZ;
static size_t           units_count         = 0;
static Unit*            units;

static Unit const*      currentUnit         = NULL;
static Function const*  currentFunction     = NULL;

static size_t           functionStack_cap   = BUFSIZ;
static size_t           functionStack_size  = 0;
static Function const*  functionStack;

static SVMap            unitMap;
static SVMap            functionMap;
static SVMap            variableMap;

static struct srcsax_context* context       = NULL;

static void free_eventElements(void) {
    free(variables);
    free(functions);
    free(units);
    free(functionStack);
    free_svmap(unitMap);
    free_svmap(functionMap);
    free_svmap(variableMap);
    srcsax_free_context(context);
}

static Event* eventsAtStartDocument[METRICS_COUNT_MAX]  = { NULL };
static Event* eventsAtEndDocument[METRICS_COUNT_MAX]    = { NULL };
static Event* eventsAtStartRoot[METRICS_COUNT_MAX]      = { NULL };
static Event* eventsAtStartUnit[METRICS_COUNT_MAX]      = { NULL };
static Event* eventsAtStartElement[METRICS_COUNT_MAX]   = { NULL };
static Event* eventsAtEndRoot[METRICS_COUNT_MAX]        = { NULL };
static Event* eventsAtEndUnit[METRICS_COUNT_MAX]        = { NULL };
static Event* eventsAtEndElement[METRICS_COUNT_MAX]     = { NULL };
static Event* eventsAtCharactersRoot[METRICS_COUNT_MAX] = { NULL };
static Event* eventsAtCharactersUnit[METRICS_COUNT_MAX] = { NULL };
static Event* eventsAtComment[METRICS_COUNT_MAX]        = { NULL };
static Event* eventsAtCDataBlock[METRICS_COUNT_MAX]     = { NULL };
static Event* eventsAtProcInfo[METRICS_COUNT_MAX]       = { NULL };

static void event_startDocument(struct srcsax_context* context) {
    /* Execute all related events */
    for (Event** event = eventsAtStartDocument; *event; event++)
        (**event)(context);
}
static void event_endDocument(struct srcsax_context* context) {
    /* Execute all related events */
    for (Event** event = eventsAtEndDocument; *event; event++)
        (**event)(context);
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
    for (Event** event = eventsAtStartRoot; *event; event++)
        (**event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
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

    currentUnit = NULL;
    for (struct srcsax_attribute const* attribute = attributes + num_attributes - 1; attribute >= attributes; attribute--) {
        unless (str_eq_const(attribute->localname, "filename")) continue;
        currentUnit = units[units_count++] = (Unit){ attribute->value };
    }
    unless (currentUnit) { fputs("PARSE_ERROR\n", stderr); exit(EXIT_FAILURE); }

    unless (insert_svmap(&unitMap, localname, (value_t){ .as_pointer = currentUnit }))
        { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    /* Execute all related events */
    for (Event** event = eventsAtStartUnit; *event; event++)
        (**event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit);
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
    /* Execute all related events */
    for (Event** event = eventsAtStartElement; *event; event++)
        (**event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes, currentUnit, currentFunction);
}
static void event_endRoot(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    /* Execute all related events */
    for (Event** event = eventsAtRoot; *event; event++)
        (**event)(context, localname, prefix, uri);
}
static void event_endUnit(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    currentUnit = NULL;

    /* Execute all related events */
    for (Event** event = eventsAtEndUnit; *event; event++)
        (**event)(context, localname, prefix, uri);
}
static void event_endElement(
    struct srcsax_context*          context,
    char const*                     localname,
    char const*                     prefix,
    char const*                     uri
) {
    /* Execute all related events */
    for (Event** event = eventsAtEndElement; *event; event++)
        (**event)(context, localname, prefix, uri);
}
static void event_charactersRoot(struct srcsax_context* context, char const* ch, int len) {
    /* Execute all related events */
    for (Event** event = eventsAtCharactersRoot; *event; event++)
        (**event)(context, ch, len);
}
static void event_charactersUnit(struct srcsax_context* context, char const* ch, int len) {
    /* Execute all related events */
    for (Event** event = eventsAtCharactersUnit; *event; event++)
        (**event)(context, ch, len);
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
    for (Event** event = eventsAtMetaTag; *event; event++)
        (**event)(context, localname, prefix, uri, num_namespaces, namespaces, num_attributes, attributes);
}
static void event_comment(struct srcsax_context* context, char const* value) {
    /* Execute all related events */
    for (Event** event = eventsAtComment; *event; event++)
        (**event)(context, ch, value);
}
static void event_cdataBlock(struct srcsax_context * context, char const* value, int len) {
    /* Execute all related events */
    for (Event** event = eventsAtCDataBlock; *event; event++)
        (**event)(context, value, len);
}
static void event_procInfo(struct srcsax_context* context, char const* target, char const* data) {
    /* Execute all related events */
    for (Event** event = eventsAtProcInfo; *event; event++)
        (**event)(context, target, data);
}

static struct srcsax_handler* events = {
    event_startDocument, event_endDocument,
    event_startRoot, event_startUnit, event_startElement,
    event_endRoot, event_endUnit, event_endElement,
    event_charactersRoot, event_charactersUnit,
    event_metaTag, event_comment, event_cdataBlock, event_procInfo
};

bool register_all_events(char* archiveBuffer, size_t const archiveSize) {
    unless (
        (context = srcsax_create_context_memory(archiveBuffer, archiveSize, NULL))          &&
        (variables = calloc(variables_cap, sizeof(Variable)))                               &&
        (functions = calloc(functions_cap, sizeof(Function)))                               &&
        (units = calloc(units_cap, sizeof(Unit)))                                           &&
        (unitMap = constructEmpty_svmap(BUFSIZ))                                            &&
        (functionMap = constructEmpty_svmap(BUFSIZ))                                        &&
        (variableMap = constructEmpty_svmap(BUFSIZ))
    ) return 0;

    /* Free all these allocations at the end */
    atexit(free_eventElements);

    /* In this context, we handle our events */
    context->handler = events;

    return 1;
}
