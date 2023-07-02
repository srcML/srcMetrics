/**
 * @file sloc.c
 * @brief Source Lines of Code
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/elements/function.h"
#include "srcmetrics/elements/unit.h"
#include "srcmetrics/elements/variable.h"
#include "srcmetrics/metrics/sloc.h"
#include "util/chunk.h"
#include "util/repeat.h"
#include "util/streq.h"
#include "util/unless.h"

#define ENTRY_COUNT_GUESS   (UNIT_COUNT_GUESS + FN_COUNT_GUESS + VAR_COUNT_GUESS)

static SVMap            sloc_statistics = NOT_AN_SVMAP;

static value_t_unsigned sloc_overall;
static value_t_unsigned sloc_currentUnit;
static value_t_unsigned sloc_currentFunction;

static void free_sloc_statistics(void) {
    free_svmap(sloc_statistics);
}

void event_startDocument_sloc(struct srcsax_context* context, ...) {
    unless (
        isValid_svmap(sloc_statistics) ||
        isValid_svmap(sloc_statistics = constructEmpty_svmap(ENTRY_COUNT_GUESS))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    atexit(free_sloc_statistics);
    sloc_overall = 0;
}

void event_endDocument_sloc(struct srcsax_context* context, ...) {
    unless (insert_svmap(&sloc_statistics, "SLOC", VAL_UNSIGNED(sloc_overall))) {
        fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE);
    }
}

void event_startUnit_sloc(struct srcsax_context* context, ...) {
    sloc_currentUnit = 0;
}

void event_endUnit_sloc(struct srcsax_context* context, ...) {
    Unit const* currentUnit;
    char const* slocKey;
    va_list args;

    va_start(args, context);
    /* Unused variables: localname, prefix, uri. */
    repeat (3) va_arg(args, char const*);
    currentUnit = va_arg(args, Unit const*);
    va_end(args);

    unless (
        (slocKey = add_chunk(&strings, "SLOC_", 5))                             &&
        append_chunk(&strings, currentUnit->name, strlen(currentUnit->name))    &&
        insert_svmap(&sloc_statistics, slocKey, VAL_UNSIGNED(sloc_currentUnit))
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
    } else if (
        str_eq_const(localname, "expr_stmt") ||
        str_eq_const(localname, "decl_stmt") ||
        str_eq_const(localname, "return")
    ) {
        sloc_overall++;
        sloc_currentUnit++;
        sloc_currentFunction++;
    }
}

void event_endElement_sloc(struct srcsax_context* context, ...) {
    char const*     localname;
    Function const* currentFunction;
    char const*     slocKey;
    va_list args;

    va_start(args, context);
    localname = va_arg(args, char const*);
    /* Unused variables: prefix, uri, currentUnit. */
    repeat (3) va_arg(args, char const*);
    currentFunction = va_arg(args, Function const*);
    va_end(args);

    unless (str_eq_const(localname, "function")) return;

    unless (
        (slocKey = add_chunk(&strings, "SLOC_", 5))                                                 &&
        append_chunk(&strings, currentFunction->designator, strlen(currentFunction->designator))    &&
        insert_svmap(&sloc_statistics, slocKey, VAL_UNSIGNED(sloc_currentFunction))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
}

SVMap* report_sloc(void) { return isValid_svmap(sloc_statistics) ? &sloc_statistics : NULL; }
