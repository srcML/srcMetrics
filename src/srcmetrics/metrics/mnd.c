/**
 * @file mnd.c
 * @brief Maximum Nesting Depth
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/mnd.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

#define MND_READ_STATE_WAITING_METHOD 0U
#define MND_READ_STATE_READING_METHOD 1U
static unsigned mnd_read_state = MND_READ_STATE_WAITING_METHOD;

static Map mnd_statistics[1]   = { NOT_A_MAP };

static unsigned mnd_overall    = 0U;
static unsigned mnd_unit       = 0U;
static unsigned mnd_fn         = 0U;
static unsigned nd             = 0U;

static void free_mnd_statistics(void) {
    VERBOSE_MSG_LITERAL("MND_FREE");
    DEBUG_ABORT_IF(!free_map(mnd_statistics))
    NDEBUG_EXECUTE(free_map(mnd_statistics))
}

void event_startDocument_mnd(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("MND_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(
            constructEmpty_map(mnd_statistics, ENTRY_COUNT_GUESS)
        )

        DEBUG_ERROR_IF(atexit(free_mnd_statistics) != 0)
        NDEBUG_EXECUTE(atexit(free_mnd_statistics))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(mnd_statistics))
    }

    mnd_overall     = 0U;
    mnd_read_state  = MND_READ_STATE_WAITING_METHOD;
}

void event_endDocument_mnd(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("MND_END => document");
    uint32_t const key_id = add_chunk(strings, "MND", 3);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(mnd_statistics, key_id, VAL_UNSIGNED(mnd_overall)))
}

void event_startUnit_mnd(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("MND_START => unit");
    mnd_read_state = MND_READ_STATE_WAITING_METHOD;
    mnd_unit       = 0U;
    nd             = 0U;
}

void event_endUnit_mnd(struct srcsax_context* context, ...) {
    va_list     args;

    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("MND_END => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    uint32_t const key_id = add_chunk(strings, "MND_", 4);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(mnd_statistics, key_id, VAL_UNSIGNED(mnd_unit)))
}

void event_startElement_mnd(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_LITERAL("MND_START => function");
        mnd_read_state = MND_READ_STATE_READING_METHOD;
        mnd_fn         = nd;
    } else if (STR_EQ_CONST(localname, "block")) {
        VERBOSE_MSG_LITERAL("MND_ND++ (block)");
        nd++;
        if (nd > mnd_overall)                                               mnd_overall = nd;
        if (nd > mnd_unit)                                                  mnd_unit    = nd;
        if (mnd_read_state == MND_READ_STATE_READING_METHOD && nd > mnd_fn) mnd_fn      = nd;
    }
}

void event_endElement_mnd(struct srcsax_context* context, ...) {
    va_list args;
    va_start(args, context);

    char const* const localname = va_arg(args, char const*);

    /* prefix and uri */
    REPEAT(2) va_arg(args, char const*);

    /* unit_id */
    va_arg(args, uint32_t);

    uint32_t const fn_id = va_arg(args, uint32_t);

    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("MND_END => function (%s)", get_chunk(strings, fn_id));
        uint32_t const key_id = add_chunk(strings, "MND_", 4);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(mnd_statistics, key_id, VAL_UNSIGNED(mnd_fn)))
        mnd_read_state = MND_READ_STATE_WAITING_METHOD;
    } else if (STR_EQ_CONST(localname, "block")) {
        VERBOSE_MSG_LITERAL("MND_ND-- (block)");
        nd--;
    }
}

Map const* report_mnd(void) {
    VERBOSE_MSG_LITERAL("MND_REPORT");
    return isValid_map(mnd_statistics) ? mnd_statistics : NULL;
}
