/**
 * @file sloc.c
 * @brief Source Lines of Code
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/metrics/sloc.h"
#include "util/chunk.h"
#include "util/repeat.h"
#include "util/streq.h"
#include "util/unless.h"

extern Chunk    strings;
static SVMap    sloc_statistics = NOT_AN_SVMAP;

static double   sloc_overall;
static double   sloc_currentUnit;
static double   sloc_currentFunction;

void event_startDocument_sloc(struct srcsax_context* context, ...) {
    unless (
        isValid_svmap(sloc_statistics) ||
        isValid_svmap(sloc_statistics = constructEmpty_svmap(BUFSIZ))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    sloc_overall = 0;
}

void event_endDocument_sloc(struct srcsax_context* context, ...) {
    unless (insert_svmap(&sloc_statistics, "SLOC", (value_t){ .as_double = sloc_overall })) {
        fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE);
    }
}

void event_startUnit_sloc(struct srcsax_context* context, ...) {
    sloc_currentUnit = 0;
}

void event_endUnit_sloc(struct srcsax_context* context, ...) {
    char const* currentUnit;
    char const* slocKey;
    va_list args;

    va_start(args, context);
    /* Unused variables: localname, prefix, uri. */
    repeat (3) va_arg(args, char const*);
    currentUnit = va_arg(args, char const*);
    va_end(args);

    unless (
        (slocKey = add_chunk(&strings, "SLOC_"))    &&
        append_chunk(&strings, currentUnit)         &&
        insert_svmap(&sloc_statistics, slocKey, (value_t){ .as_double = sloc_currentUnit })
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
}

void event_startElement_sloc(struct srcsax_context* context, ...) {
    char const* localname;
    va_list args;

    va_start(args, context);
    localname = va_arg(args, char const*);
    va_end(args);

    if (str_eq_const(localname, "function")) {
        sloc_currentFunction = 0;
    } else if (str_eq_const(localname, "expr_stmt")) {
        sloc_overall++;
        sloc_currentUnit++;
        sloc_currentFunction++;
    }
}

void event_endElement_sloc(struct srcsax_context* context, ...) {
    char const* localname;
    char const* currentUnit;
    char const* currentFunction;
    char const* slocKey;
    va_list args;

    va_start(args, context);
    localname = va_arg(args, char const*);
    repeat (2) va_arg(args, char const*);
    currentUnit = va_arg(args, char const*);
    currentFunction = va_arg(args, char const*);
    va_end(args);

    unless (str_eq_const(localname, "function")) return;

    unless (
        (slocKey = add_chunk(&strings, "SLOC_") &&
        append_chunk(&strings, currentUnit)     &&
        append_chunk(&strings, currentFunction) &&
        insert_svmap(&sloc_statistics, slocKey, (value_t){ .as_double = sloc_currentFunction })
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
}

SVMap* report_sloc(void) { return isValid_svmap(sloc_statistics) ? &sloc_statistics : NULL; }
