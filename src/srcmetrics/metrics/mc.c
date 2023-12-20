/**
 * @file mc.c
 * @brief Method Count
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/mc.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS UNIT_COUNT_GUESS

static Map mc_statistics[1] = { NOT_A_MAP };

static unsigned mc_overall  = 0U;
static unsigned mc_unit     = 0U;

static void free_mc_statistics(void) {
    VERBOSE_MSG_LITERAL("MC_FREE");
    DEBUG_ABORT_IF(!free_map(mc_statistics))
    NDEBUG_EXECUTE(free_map(mc_statistics))
}

void event_startDocument_mc(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("MC_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(mc_statistics, ENTRY_COUNT_GUESS))

        DEBUG_ERROR_IF(atexit(free_mc_statistics) != 0)
        NDEBUG_EXECUTE(atexit(free_mc_statistics))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(mc_statistics))
    }

    mc_overall = 0U;
}

void event_endDocument_mc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("MC_END => document");
    uint32_t const key_id = add_chunk(strings, "MC", 2);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(mc_statistics, key_id, VAL_UNSIGNED(mc_overall)))
}

void event_startUnit_mc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("MC_START => unit");
    mc_unit = 0U;
}

void event_endUnit_mc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* Unused variables: localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("MC_END => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    uint32_t const key_id = add_chunk(strings, "MC_", 3);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(mc_statistics, key_id, VAL_UNSIGNED(mc_unit)))
}

void event_startElement_mc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_LITERAL("MC++ (function)");
        mc_overall++;
        mc_unit++;
    }
}

Map const* report_mc(void) {
    VERBOSE_MSG_LITERAL("MC_REPORT");
    return isValid_map(mc_statistics) ? mc_statistics : NULL;
}
