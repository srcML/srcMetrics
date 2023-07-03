/**
 * @file npm.c
 * @brief Number of Public Methods
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/elements/function.h"
#include "srcmetrics/elements/unit.h"
#include "srcmetrics/elements/variable.h"
#include "srcmetrics/metrics/npm.h"
#include "util/chunk.h"
#include "util/repeat.h"
#include "util/streq.h"
#include "util/unless.h"

#define ENTRY_COUNT_GUESS   (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

static int              npm_read_state = 0;
static SVMap            npm_statistics = NOT_AN_SVMAP;

static value_t_unsigned npm_overall;
static value_t_unsigned npm_currentUnit;

static void free_npm_statistics(void) { free_svmap(npm_statistics); }

void event_startDocument_npm(struct srcsax_context* context, ...) {
    unless (
        isValid_svmap(npm_statistics) ||
        isValid_svmap(npm_statistics = constructEmpty_svmap(ENTRY_COUNT_GUESS))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    atexit(free_npm_statistics);
    npm_overall = 0;
}

void event_endDocument_npm(struct srcsax_context* context, ...) {
    unless (insert_svmap(&npm_statistics, "NPM", VAL_UNSIGNED(npm_overall))) {
        fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE);
    }
}

void event_startUnit_npm(struct srcsax_context* context, ...) {
    npm_currentUnit = 0;
}

void event_endUnit_npm(struct srcsax_context* context, ...) {
    Unit const* currentUnit;
    char const* npmKey;
    va_list args;

    va_start(args, context);
    /* Unused variables: localname, prefix, uri. */
    repeat (3) va_arg(args, char const*);
    currentUnit = va_arg(args, Unit const*);
    va_end(args);

    unless (
        (npmKey = add_chunk(&strings, "NPM_", 4))                               &&
        append_chunk(&strings, currentUnit->name, strlen(currentUnit->name))    &&
        insert_svmap(&npm_statistics, npmKey, VAL_UNSIGNED(npm_currentUnit))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
}

void event_startElement_npm(struct srcsax_context* context, ...) {
    char const* localname;
    va_list args;

    va_start(args, context);
    localname = va_arg(args, char const*);
    va_end(args);

    if (str_eq_const(localname, "function")) {
        npm_read_state = 1;
        npm_overall++;
        npm_currentUnit++;
    } else if (str_eq_const(localname, "type")) {
        npm_read_state = 2;
    } else if (str_eq_const(localname, "specifier")) {
        npm_read_state = 3;
    }
}

void event_endElement_npm(struct srcsax_context* context, ...) {
    char const* localname;
    va_list args;

    va_start(args, context);
    localname = va_arg(args, char const*);
    va_end(args);

    switch (npm_read_state) {
        case 2:
            if (str_eq_const(localname, "type")) npm_read_state = 0;
            break;
        case 3:
            if (str_eq_const(localname, "specifier")) npm_read_state = 0;
            break;
        default:
            if (str_eq_const(localname, "function")) npm_read_state = 0;
    }
}

void event_charactersUnit_npm(struct srcsax_context* context, ...) {
    char const* ch;
    va_list args;

    va_start(args, context);
    ch = va_arg(args, char const*);
    va_end(args);

    if (npm_read_state == 3 && str_eq_const(ch, "static")) {
        npm_read_state = 0;
        npm_overall -= !!npm_overall;
        npm_currentUnit -= !!npm_currentUnit;
    }
}

SVMap* report_npm(void) { return isValid_svmap(npm_statistics) ? &npm_statistics : NULL; }
