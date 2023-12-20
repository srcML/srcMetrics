/**
 * @file abc.c
 * @brief Assignments, Branches, Conditionals
 * @author Yavuz Koroglu
 */
#include <math.h>
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/abc.h"
#include "padkit/chunk.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

static Map abc_statistics[1] = { NOT_A_MAP };

#define AC_READ_STATE_WAITING_FOR_OPERATOR 0U
#define AC_READ_STATE_COMMENT              1U
#define AC_READ_STATE_READING_OPERATOR     2U
#define AC_READ_STATE_DECLARATION          3U
static unsigned ac_read_state = AC_READ_STATE_WAITING_FOR_OPERATOR;

static unsigned a_overall   = 0U;
static unsigned b_overall   = 0U;
static unsigned c_overall   = 0U;
static unsigned a_unit      = 0U;
static unsigned b_unit      = 0U;
static unsigned c_unit      = 0U;
static unsigned a_fn        = 0U;
static unsigned b_fn        = 0U;
static unsigned c_fn        = 0U;
static float    abc_overall = 0.0f;
static float    abc_unit    = 0.0f;
static float    abc_fn      = 0.0f;

static Chunk    op_chunk[1] = { NOT_A_CHUNK };

static void free_abc_stuff(void) {
    VERBOSE_MSG_LITERAL("ABC_FREE");

    DEBUG_ABORT_IF(!free_map(abc_statistics))
    NDEBUG_EXECUTE(free_map(abc_statistics))

    DEBUG_ABORT_IF(!free_chunk(op_chunk))
    NDEBUG_EXECUTE(free_chunk(op_chunk))
}

void event_startDocument_abc(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("ABC_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(
            constructEmpty_map(abc_statistics, ENTRY_COUNT_GUESS)
        )

        DEBUG_ASSERT_NDEBUG_EXECUTE(
            constructEmpty_chunk(op_chunk, BUFSIZ, 1)
        )

        DEBUG_ERROR_IF(atexit(free_abc_stuff) != 0)
        NDEBUG_EXECUTE(atexit(free_abc_stuff))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(abc_statistics))

        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(op_chunk))
    }

    ac_read_state   = AC_READ_STATE_WAITING_FOR_OPERATOR;
    a_overall       = 0U;
    b_overall       = 0U;
    c_overall       = 0U;
    abc_overall     = 0.0f;
}

void event_endDocument_abc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("ABC_END => document");

    abc_overall
        = sqrtf((float)((a_overall * a_overall) + (b_overall * b_overall) + (c_overall * c_overall)));

    uint32_t key_id = add_chunk(strings, "ABC-A", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(a_overall)))

    key_id = add_chunk(strings, "ABC-B", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(b_overall)))

    key_id = add_chunk(strings, "ABC-C", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(c_overall)))

    key_id = add_chunk(strings, "ABC", 3);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_FLOAT(abc_overall)))
}

void event_startUnit_abc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("ABC_START => unit");

    ac_read_state = AC_READ_STATE_WAITING_FOR_OPERATOR;
    a_unit        = 0U;
    b_unit        = 0U;
    c_unit        = 0U;
    abc_unit      = 0.0f;
}

void event_endUnit_abc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    va_end(args);

    VERBOSE_MSG_VARIADIC("ABC_END => unit (%s)", get_chunk(strings, unit_id));

    abc_unit
        = sqrtf((float)((a_unit * a_unit) + (b_unit * b_unit) + (c_unit * c_unit)));

    uint32_t key_id = add_chunk(strings, "ABC-A_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(a_unit)))

    key_id = add_chunk(strings, "ABC-B_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(b_unit)))

    key_id = add_chunk(strings, "ABC-C_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(c_unit)))

    key_id = add_chunk(strings, "ABC_", 4);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_FLOAT(abc_unit)))
}

void event_startElement_abc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    char const* const localname = va_arg(args, char const*);

    /* prefix, uri */
    REPEAT(2) va_arg(args, char const*);

    /* num_namespaces */
    va_arg(args, int);

    /* namespaces */
    va_arg(args, void const*);

    /* num_attributes */
    va_arg(args, int);

    /* attributes */
    va_arg(args, void const*);

    uint32_t const unit_id = va_arg(args, uint32_t);
    uint32_t const fn_id   = va_arg(args, uint32_t);

    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("ABC_START => %s", localname);
        a_fn   = 0U;
        b_fn   = 0U;
        c_fn   = 0U;
        abc_fn = 0.0f;
    } else if (STR_EQ_CONST(localname, "call") || STR_EQ_CONST(localname, "goto")) {
        VERBOSE_MSG_VARIADIC("ABC_BRANCHES++ => %s", localname);
        b_overall++;
        b_unit += (unit_id != 0xFFFFFFFF);
        b_fn   += (fn_id   != 0xFFFFFFFF);
    } else if (STR_EQ_CONST(localname, "comment")) {
        VERBOSE_MSG_VARIADIC("ABC_START => %s", localname);
        ac_read_state = AC_READ_STATE_COMMENT;
    } else if (
        STR_EQ_CONST(localname, "else")     ||
        STR_EQ_CONST(localname, "case")     ||
        STR_EQ_CONST(localname, "default")  ||
        STR_EQ_CONST(localname, "ternary")
    ) {
        VERBOSE_MSG_VARIADIC("ABC_CONDITIONALS++ => %s", localname);
        c_overall++;
        c_unit += (unit_id != 0xFFFFFFFF);
        c_fn   += (fn_id   != 0xFFFFFFFF);
    } else if (STR_EQ_CONST(localname, "operator")) {
        VERBOSE_MSG_VARIADIC("ABC_START => %s", localname);
        ac_read_state = AC_READ_STATE_READING_OPERATOR;
    } else if (STR_EQ_CONST(localname, "decl_stmt")) {
        ac_read_state = AC_READ_STATE_DECLARATION;
    } else if (ac_read_state == AC_READ_STATE_DECLARATION && STR_EQ_CONST(localname, "init")) {
        VERBOSE_MSG_LITERAL("ABC_ASSIGNMENTS++ => (=)");
        a_overall++;
        a_unit += (unit_id != 0xFFFFFFFF);
        a_fn   += (fn_id != 0xFFFFFFFF);
        ac_read_state = AC_READ_STATE_WAITING_FOR_OPERATOR;
    }
}

void event_endElement_abc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    char const* const localname = va_arg(args, char const*);

    /* prefix and uri */
    REPEAT(2) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);
    uint32_t const fn_id   = va_arg(args, uint32_t);

    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("ABC_END => function (%s)", get_chunk(strings, fn_id));
        abc_fn
            = sqrtf((float)((a_fn * a_fn) + (b_fn * b_fn) + (c_fn * c_fn)));

        uint32_t key_id = add_chunk(strings, "ABC-A_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(a_fn)))

        key_id = add_chunk(strings, "ABC-B_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(b_fn)))

        key_id = add_chunk(strings, "ABC-C_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_UNSIGNED(c_fn)))

        key_id = add_chunk(strings, "ABC_", 4);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(abc_statistics, key_id, VAL_FLOAT(abc_fn)))
    } else if (STR_EQ_CONST(localname, "comment")) {
        VERBOSE_MSG_VARIADIC("ABC_END => %s", localname);
        ac_read_state = AC_READ_STATE_WAITING_FOR_OPERATOR;
    } else if (STR_EQ_CONST(localname, "operator")) {
        char const* const op = getLast_chunk(op_chunk);
        DEBUG_ERROR_IF(op == NULL)

        VERBOSE_MSG_VARIADIC("ABC_END => %s (%s)", localname, op);

        if (
            STR_EQ_CONST(op, "!=") ||
            STR_EQ_CONST(op, "==") ||
            STR_EQ_CONST(op, ">=") ||
            STR_EQ_CONST(op, "<=") ||
            STR_EQ_CONST(op, ">")  ||
            STR_EQ_CONST(op, "<")
        ) {
            VERBOSE_MSG_VARIADIC("ABC_CONDITIONALS++ (%s)", op);
            c_overall++;
            c_unit += (unit_id != 0xFFFFFFFF);
            c_fn   += (fn_id != 0xFFFFFFFF);
        } else if (
            STR_EQ_CONST(op, "++")  ||
            STR_EQ_CONST(op, "--")  ||
            STR_EQ_CONST(op, "^=")  ||
            STR_EQ_CONST(op, "|=")  ||
            STR_EQ_CONST(op, "&=")  ||
            STR_EQ_CONST(op, ">>=") ||
            STR_EQ_CONST(op, "<<=") ||
            STR_EQ_CONST(op, "-=")  ||
            STR_EQ_CONST(op, "+=")  ||
            STR_EQ_CONST(op, "%=")  ||
            STR_EQ_CONST(op, "/=")  ||
            STR_EQ_CONST(op, "*=")  ||
            STR_EQ_CONST(op, "=")
        ) {
            VERBOSE_MSG_VARIADIC("ABC_ASSIGNMENTS++ (%s)", op);
            a_overall++;
            a_unit += (unit_id != 0xFFFFFFFF);
            a_fn   += (fn_id != 0xFFFFFFFF);
        }

        ac_read_state = AC_READ_STATE_WAITING_FOR_OPERATOR;
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(op_chunk))
    }
}

void event_charactersUnit_abc(struct srcsax_context* context, ...) {
    va_list args;

    if (ac_read_state != AC_READ_STATE_READING_OPERATOR) return;

    va_start(args, context);
    char const* const ch = va_arg(args, char*);
    uint64_t const len   = va_arg(args, uint64_t);
    va_end(args);

    DEBUG_ERROR_IF(append_chunk(op_chunk, ch, len) == NULL)
    NDEBUG_EXECUTE(append_chunk(op_chunk, ch, len))
}

Map const* report_abc(void) {
    VERBOSE_MSG_LITERAL("ABC_REPORT");
    return isValid_map(abc_statistics) ? abc_statistics : NULL;
}
