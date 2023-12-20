/**
 * @file ams.c
 * @brief Average Method Size
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/ams.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/reallocate.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS UNIT_COUNT_GUESS

static Map ams_statistics[1] = { NOT_A_MAP };

#define AMS_READ_STATE_WAITING_METHOD    0U
#define AMS_READ_STATE_READING_METHOD    1U
#define AMS_READ_STATE_READING_STATEMENT 2U
static unsigned  ams_read_state       = AMS_READ_STATE_WAITING_METHOD;

static unsigned  method_count_unit    = 0U;
static unsigned  method_count_overall = 0U;
static unsigned  method_size          = 0U;
static unsigned  ms_overall_cap       = FN_COUNT_GUESS;
static unsigned* ms_overall_list      = NULL;
static unsigned  ms_unit_cap          = FN_COUNT_GUESS / UNIT_COUNT_GUESS;
static unsigned* ms_unit_list         = NULL;
static float     ams_overall          = 0.0f;
static float     ams_unit             = 0.0f;

static void free_ams_stuff(void) {
    VERBOSE_MSG_LITERAL("AMS_FREE");
    DEBUG_ABORT_IF(!free_map(ams_statistics))
    NDEBUG_EXECUTE(free_map(ams_statistics))
    free(ms_overall_list);
    free(ms_unit_list);
    ms_overall_list = NULL;
    ms_unit_list    = NULL;
}

void event_startDocument_ams(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("AMS_START => document");

    if (first_time_execution) {
        DEBUG_ASSERT_NDEBUG_EXECUTE(
            constructEmpty_map(ams_statistics, ENTRY_COUNT_GUESS)
        )

        ms_overall_list = malloc(ms_overall_cap * sizeof(unsigned));
        DEBUG_ERROR_IF(ms_overall_list == NULL)

        ms_unit_list = malloc(ms_unit_cap * sizeof(unsigned));
        DEBUG_ERROR_IF(ms_unit_list == NULL)

        DEBUG_ERROR_IF(atexit(free_ams_stuff) != 0)
        NDEBUG_EXECUTE(atexit(free_ams_stuff))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(ams_statistics))
    }

    ams_read_state       = AMS_READ_STATE_WAITING_METHOD;
    ams_overall          = 0.0f;
    method_count_overall = 0U;
}

void event_endDocument_ams(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("AMS_END => document");

    unsigned sum_ms = 0U;
    for (
        unsigned const* ms = ms_overall_list + method_count_overall - 1;
        ms >= ms_overall_list;
        ms--
    ) sum_ms += *ms;

    ams_overall = (float)sum_ms / (float)method_count_overall;
    uint32_t const key_id = add_chunk(strings, "AMS", 3);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(ams_statistics, key_id, VAL_FLOAT(ams_overall)))
}

void event_startUnit_ams(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("AMS_START => unit");

    ams_read_state    = AMS_READ_STATE_WAITING_METHOD;
    method_count_unit = 0U;
    ams_unit          = 0.0f;
}

void event_endUnit_ams(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* localname, prefix, uri. */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("AMS_START => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    unsigned sum_ms = 0U;
    for (
        unsigned const* ms = ms_unit_list + method_count_unit - 1;
        ms >= ms_unit_list;
        ms--
    ) sum_ms += *ms;

    ams_unit = (float)sum_ms / (float)method_count_unit;

    uint32_t const key_id = add_chunk(strings, "AMS_", 4);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(ams_statistics, key_id, VAL_FLOAT(ams_unit)))
}

void event_startElement_ams(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    switch (ams_read_state) {
        case AMS_READ_STATE_WAITING_METHOD:
            if (!STR_EQ_CONST(localname, "function")) break;
            VERBOSE_MSG_VARIADIC("AMS_METHOD_COUNT++ (%s)", localname);
            ams_read_state = AMS_READ_STATE_READING_METHOD;
            method_size    = 0U;
            method_count_unit++;
            method_count_overall++;
            break;
        case AMS_READ_STATE_READING_METHOD:
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
            ) {
                VERBOSE_MSG_VARIADIC("AMS_METHOD_SIZE++ (%s)", localname);
                ams_read_state = AMS_READ_STATE_READING_STATEMENT;
                method_size++;
            } else if (
                STR_EQ_CONST(localname, "for")       ||
                STR_EQ_CONST(localname, "while")     ||
                STR_EQ_CONST(localname, "switch")    ||
                STR_EQ_CONST(localname, "if_stmt")   ||
                STR_EQ_CONST(localname, "typedef")   ||
                STR_EQ_CONST(localname, "struct")    ||
                STR_EQ_CONST(localname, "union")
            ) {
                VERBOSE_MSG_VARIADIC("AMS_METHOD_SIZE++ (%s)", localname);
                method_size++;
            }
    }
}

void event_endElement_ams(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    switch (ams_read_state) {
        case AMS_READ_STATE_READING_METHOD:
            if (!STR_EQ_CONST(localname, "function")) break;
            VERBOSE_MSG_LITERAL("AMS_END => function");
            ams_read_state = AMS_READ_STATE_WAITING_METHOD;

            REALLOC_IF_NECESSARY(
                unsigned, ms_overall_list,
                unsigned, ms_overall_cap, method_count_overall,
                {REALLOC_ERROR;}
            )
            ms_overall_list[method_count_overall - 1] = method_size;

            REALLOC_IF_NECESSARY(
                unsigned, ms_unit_list,
                unsigned, ms_unit_cap, method_count_unit,
                {REALLOC_ERROR;}
            )
            ms_unit_list[method_count_unit - 1] = method_size;

            break;
        case AMS_READ_STATE_READING_STATEMENT:
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
            ) ams_read_state = AMS_READ_STATE_READING_METHOD;
    }
}

Map const* report_ams(void) {
    VERBOSE_MSG_LITERAL("AMS_REPORT");
    return isValid_map(ams_statistics) ? ams_statistics : NULL;
}
