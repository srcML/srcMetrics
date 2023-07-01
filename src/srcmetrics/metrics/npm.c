/**
 * @file npm.c
 * @brief Number of Public Methods
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "srcmetrics/metrics/npm.h"
#include "util/chunk.h"
#include "util/csv.h"
#include "util/repeat.h"
#include "util/svmap.h"

extern Chunk    strings;
static int      npm_read_state = 0;
static SVMap    npm_statistics = NOT_AN_SVMAP;

static unsigned npm_overall;
static unsigned npm_currentUnit;

void event_startDocument_npm(struct srcsax_context* context, ...) {
    unless (
        isValid_svmap(npm_statistics) ||
        isValid_svmap(npm_statistics = constructEmpty_svmap(BUFSIZ))
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }

    npm_overall = 0;
}

void event_startUnit_npm(struct srcsax_context* context, ...) {
    npm_currentUnit = 0;
}

void event_endUnit_npm(struct srcsax_context* context, ...) {
    char const* currentUnit;
    char const* npmKey;
    va_list args;

    va_start(args, context);
    /* Unused variables: localname, prefix, uri. */
    repeat (3) va_arg(args, char const*);
    currentUnit = va_arg(args, char const*);
    va_end(args);

    unless (
        (npmKey = add_chunk(&strings, "NPM_"))                  &&
        append_chunk(&strings, currentUnit)                     &&
        insert_svmap(&npm_statistics, npmKey, npm_currentUnit)
    ) { fputs("MEMORY_ERROR\n", stderr); exit(EXIT_FAILURE); }
}

void event_startElement_npm(struct srcsax_context* context, ...) {

}

void event_endElement_npm(struct srcsax_context* context, ...) {

}

void event_charactersUnit_npm(struct srcsax_context* context, ...) {

}

void report_npm(FILE* output) {
    for (SVPair* pair = npm_statistics.pairs; pair < npm_statistics.size; pair++)
        fprintf(output, "%s%s%s%s", pair->key, csv_delimeter, pair->value, csv_row_end);
}
