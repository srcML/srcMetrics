/**
 * @file sloc.c
 * @brief Source Lines of Code
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/sloc.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

static Map sloc_statistics[1] = { NOT_A_MAP };

#define SLOC_STATE_WAITING_UNIT             0U
#define SLOC_STATE_READING_UNIT             1U
#define SLOC_STATE_READING_METHOD           2U
#define SLOC_STATE_READING_UNIT_STATEMENT   3U
#define SLOC_STATE_READING_METHOD_STATEMENT 4U
static unsigned sloc_state   = 0U;

static unsigned sloc_overall = 0U;
static unsigned sloc_unit    = 0U;
static unsigned sloc_fn      = 0U;

static void free_sloc_statistics(void) {
    VERBOSE_MSG_LITERAL("SLOC_FREE");

    DEBUG_ABORT_IF(!free_map(sloc_statistics))
    NDEBUG_EXECUTE(free_map(sloc_statistics))
}

void event_startDocument_sloc(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("SLOC_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(sloc_statistics, ENTRY_COUNT_GUESS))

        DEBUG_ERROR_IF(atexit(free_sloc_statistics) != 0)
        NDEBUG_EXECUTE(atexit(free_sloc_statistics))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(sloc_statistics))
    }

    sloc_overall = 0U;
    sloc_state   = SLOC_STATE_WAITING_UNIT;
}

void event_endDocument_sloc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("SLOC_END => document");
    uint32_t const key_id = add_chunk(strings, "SLOC", 4);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(sloc_statistics, key_id, VAL_UNSIGNED(sloc_overall)))
}

void event_startUnit_sloc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("SLOC_START => unit");
    sloc_unit  = 0U;
    sloc_state = SLOC_STATE_READING_UNIT;
}

void event_endUnit_sloc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("SLOC_END => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    uint32_t const key_id = add_chunk(strings, "SLOC_", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(sloc_statistics, key_id, VAL_UNSIGNED(sloc_unit)))

    sloc_state = SLOC_STATE_WAITING_UNIT;
}

void event_startElement_sloc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("SLOC++ (%s)", localname);
        sloc_overall++;
        sloc_unit++;
        sloc_fn    = 0U;
        sloc_state = SLOC_STATE_READING_METHOD;
    } else if (
        STR_EQ_CONST(localname, "expr_stmt") ||
        STR_EQ_CONST(localname, "decl_stmt") ||
        STR_EQ_CONST(localname, "return")    ||
        STR_EQ_CONST(localname, "break")     ||
        STR_EQ_CONST(localname, "continue")  ||
        STR_EQ_CONST(localname, "label")     ||
        STR_EQ_CONST(localname, "goto")      ||
        STR_EQ_CONST(localname, "default")   ||
        STR_EQ_CONST(localname, "case")      ||
        STR_EQ_CONST(localname, "directive") ||
        STR_EQ_CONST(localname, "macro")
    ) {
        switch (sloc_state) {
            case SLOC_STATE_READING_UNIT:
                VERBOSE_MSG_VARIADIC("SLOC++ (%s)", localname);
                sloc_unit++;
                sloc_overall++;
                sloc_state = SLOC_STATE_READING_UNIT_STATEMENT;
                break;
            case SLOC_STATE_READING_METHOD:
                VERBOSE_MSG_VARIADIC("SLOC++ (%s)", localname);
                sloc_unit++;
                sloc_overall++;
                sloc_fn++;
                sloc_state = SLOC_STATE_READING_METHOD_STATEMENT;
        }
    } else if (
        STR_EQ_CONST(localname, "for")     ||
        STR_EQ_CONST(localname, "while")   ||
        STR_EQ_CONST(localname, "switch")  ||
        STR_EQ_CONST(localname, "if_stmt") ||
        STR_EQ_CONST(localname, "typedef") ||
        STR_EQ_CONST(localname, "struct")  ||
        STR_EQ_CONST(localname, "union")
    ) {
        switch (sloc_state) {
            case SLOC_STATE_READING_METHOD:
                VERBOSE_MSG_VARIADIC("SLOC++ (%s)", localname);
                sloc_overall++;
                sloc_unit++;
                sloc_fn++;
        }
    }
}

void event_endElement_sloc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);

    /* prefix, uri */
    REPEAT(2) va_arg(args, char const*);

    /* unit_id */
    va_arg(args, uint32_t);

    uint32_t const fn_id = va_arg(args, uint32_t);
    va_end(args);

    switch (sloc_state) {
        case SLOC_STATE_READING_METHOD_STATEMENT:
            if (
                STR_EQ_CONST(localname, "expr_stmt") ||
                STR_EQ_CONST(localname, "decl_stmt") ||
                STR_EQ_CONST(localname, "return")    ||
                STR_EQ_CONST(localname, "break")     ||
                STR_EQ_CONST(localname, "continue")  ||
                STR_EQ_CONST(localname, "label")     ||
                STR_EQ_CONST(localname, "goto")      ||
                STR_EQ_CONST(localname, "default")   ||
                STR_EQ_CONST(localname, "case")      ||
                STR_EQ_CONST(localname, "directive") ||
                STR_EQ_CONST(localname, "macro")
            ) sloc_state = SLOC_STATE_READING_METHOD;
            break;
        case SLOC_STATE_READING_UNIT_STATEMENT:
            if (
                STR_EQ_CONST(localname, "expr_stmt") ||
                STR_EQ_CONST(localname, "decl_stmt") ||
                STR_EQ_CONST(localname, "return")    ||
                STR_EQ_CONST(localname, "break")     ||
                STR_EQ_CONST(localname, "continue")  ||
                STR_EQ_CONST(localname, "label")     ||
                STR_EQ_CONST(localname, "goto")      ||
                STR_EQ_CONST(localname, "default")   ||
                STR_EQ_CONST(localname, "case")      ||
                STR_EQ_CONST(localname, "directive") ||
                STR_EQ_CONST(localname, "macro")
            ) sloc_state = SLOC_STATE_READING_UNIT;
    }

    if (!STR_EQ_CONST(localname, "function")) return;

    sloc_state = SLOC_STATE_READING_UNIT;
    uint32_t const key_id = add_chunk(strings, "SLOC_", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(sloc_statistics, key_id, VAL_UNSIGNED(sloc_fn)))
}

Map const* report_sloc(void) {
    VERBOSE_MSG_LITERAL("SLOC_REPORT");
    return isValid_map(sloc_statistics) ? sloc_statistics : NULL;
}
