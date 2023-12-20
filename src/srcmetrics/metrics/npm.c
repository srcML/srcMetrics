/**
 * @file npm.c
 * @brief Number of Public Methods
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/npm.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS UNIT_COUNT_GUESS

#define NPM_READ_STATE_WAITING_METHOD    0U
#define NPM_READ_STATE_READING_METHOD    1U
#define NPM_READ_STATE_READING_TYPE      2U
#define NPM_READ_STATE_READING_SPECIFIER 3U
static unsigned npm_read_state  = NPM_READ_STATE_WAITING_METHOD;

static Map npm_statistics[1]    = { NOT_A_MAP };

static unsigned npm_overall     = 0U;
static unsigned npm_unit        = 0U;

static Chunk specifier_chunk[1] = { NOT_A_CHUNK };

static void free_npm_stuff(void) {
    VERBOSE_MSG_LITERAL("NPM_FREE");

    DEBUG_ABORT_IF(!free_map(npm_statistics))
    NDEBUG_EXECUTE(free_map(npm_statistics))

    DEBUG_ABORT_IF(!free_chunk(specifier_chunk))
    NDEBUG_EXECUTE(free_chunk(specifier_chunk))
}

void event_startDocument_npm(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("NPM_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(npm_statistics, ENTRY_COUNT_GUESS))
        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_chunk(specifier_chunk, BUFSIZ, 1))

        DEBUG_ERROR_IF(atexit(free_npm_stuff) != 0)
        NDEBUG_EXECUTE(atexit(free_npm_stuff))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(npm_statistics))
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(specifier_chunk))
    }

    npm_overall     = 0U;
    npm_read_state  = NPM_READ_STATE_WAITING_METHOD;
}

void event_endDocument_npm(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("NPM_END => document");
    uint32_t const key_id = add_chunk(strings, "NPM", 3);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(npm_statistics, key_id, VAL_UNSIGNED(npm_overall)))
}

void event_startUnit_npm(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("NPM_START => unit");
    npm_read_state  = NPM_READ_STATE_WAITING_METHOD;
    npm_unit        = 0U;
}

void event_endUnit_npm(struct srcsax_context* context, ...) {
    va_list args;
    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("NPM_END => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    uint32_t const key_id = add_chunk(strings, "NPM_", 4);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(npm_statistics, key_id, VAL_UNSIGNED(npm_unit)))
}

void event_startElement_npm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    switch (npm_read_state) {
        case NPM_READ_STATE_WAITING_METHOD:
            if (STR_EQ_CONST(localname, "function")) {
                VERBOSE_MSG_LITERAL("NPM++ (function)");
                npm_read_state = NPM_READ_STATE_READING_METHOD;
                npm_overall++;
                npm_unit++;
            }
            break;
        case NPM_READ_STATE_READING_METHOD:
            if (STR_EQ_CONST(localname, "type")) {
                npm_read_state = NPM_READ_STATE_READING_TYPE;
            }
            break;
        case NPM_READ_STATE_READING_TYPE:
            if (STR_EQ_CONST(localname, "specifier")) {
                npm_read_state = NPM_READ_STATE_READING_SPECIFIER;
            }
            break;
    }
}

void event_endElement_npm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    switch (npm_read_state) {
        case NPM_READ_STATE_READING_TYPE:
            if (STR_EQ_CONST(localname, "type"))
                npm_read_state = NPM_READ_STATE_WAITING_METHOD;
            break;
        case NPM_READ_STATE_READING_SPECIFIER:
            if (STR_EQ_CONST(localname, "specifier")) {
                char const* const specifier = getLast_chunk(specifier_chunk);
                DEBUG_ERROR_IF(specifier == NULL)

                if (STR_EQ_CONST(specifier, "static")) {
                    VERBOSE_MSG_LITERAL("NPM-- (static function)");
                    npm_read_state = NPM_READ_STATE_WAITING_METHOD;
                    npm_overall--;
                    npm_unit--;
                }
                DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(specifier_chunk))
            }
            break;
        default:
            if (STR_EQ_CONST(localname, "function"))
                npm_read_state = NPM_READ_STATE_WAITING_METHOD;
    }
}

void event_charactersUnit_npm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const ch = va_arg(args, char const*);
    uint64_t const len   = va_arg(args, uint64_t);
    va_end(args);

    if (npm_read_state == NPM_READ_STATE_READING_SPECIFIER)
        DEBUG_ERROR_IF(append_chunk(specifier_chunk, ch, len) == NULL)
        NDEBUG_EXECUTE(append_chunk(specifier_chunk, ch, len))
}

Map const* report_npm(void) {
    VERBOSE_MSG_LITERAL("NPM_REPORT");
    return isValid_map(npm_statistics) ? npm_statistics : NULL;
}
