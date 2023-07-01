/**
 * @file npm.c
 */
#include <stdarg.h>
#include "srcmetrics/metrics/npm.h"
#include "util/chunk.h"
#include "util/svmap.h"

extern Chunk    chunk;
static int      npm_read_state = 0;
static SVMap    npm_statistics = NOT_AN_SVMAP;

