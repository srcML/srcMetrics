/**
 * @file hsm.c
 * @brief Assignments, Branches, Conditionals
 * @author Yavuz Koroglu
 */
#include <math.h>
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/hsm.h"
#include "padkit/chunkset.h"
#include "padkit/debug.h"
#include "padkit/reallocate.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

#define HSM_READ_STATE_WAITING          0U
#define HSM_READ_STATE_READING_OPERATOR 1U
static unsigned hsm_read_state          = HSM_READ_STATE_WAITING;

static Map hsm_statistics[1]            = { NOT_A_MAP };

#define HSM_OPERATORS_OVERALL           0
#define HSM_OPERATORS_UNIT              1
#define HSM_OPERATORS_FN                2
#define HSM_OPERANDS_OVERALL            3
#define HSM_OPERANDS_UNIT               4
#define HSM_OPERANDS_FN                 5
#define HSM_SET_LAST                    HSM_OPERANDS_FN
static ChunkSet set[HSM_SET_LAST + 1] = {
    NOT_A_CHUNK_SET, NOT_A_CHUNK_SET, NOT_A_CHUNK_SET,
    NOT_A_CHUNK_SET, NOT_A_CHUNK_SET, NOT_A_CHUNK_SET
};

/* # Distinct Operators */
static unsigned nu1_overall             = 0U;
static unsigned nu1_unit                = 0U;
static unsigned nu1_fn                  = 0U;

/* # Distinct Operands */
static unsigned nu2_overall             = 0U;
static unsigned nu2_unit                = 0U;
static unsigned nu2_fn                  = 0U;

/* # Operators */
static unsigned n1_overall              = 0U;
static unsigned n1_unit                 = 0U;
static unsigned n1_fn                   = 0U;

/* # Operands */
static unsigned n2_overall              = 0U;
static unsigned n2_unit                 = 0U;
static unsigned n2_fn                   = 0U;

/* Halstead Vocabulary */
static unsigned nu_overall              = 0U;
static unsigned nu_unit                 = 0U;
static unsigned nu_fn                   = 0U;

/* Halstead Length */
static unsigned n_overall               = 0U;
static unsigned n_unit                  = 0U;
static unsigned n_fn                    = 0U;

/* Halstead Bugs */
static float b_overall                  = 0.0f;
static float b_unit                     = 0.0f;
static float b_fn                       = 0.0f;

/* Halstead Time */
static float t_overall                  = 0.0f;
static float t_unit                     = 0.0f;
static float t_fn                       = 0.0f;

/* Halstead Volume */
static float v_overall                  = 0.0f;
static float v_unit                     = 0.0f;
static float v_fn                       = 0.0f;

/* Halstead Difficulty */
static float d_overall                  = 0.0f;
static float d_unit                     = 0.0f;
static float d_fn                       = 0.0f;

/* Halstead Effort */
static float e_overall                  = 0.0f;
static float e_unit                     = 0.0f;
static float e_fn                       = 0.0f;

static Chunk* chunk_stack               = NULL;
static unsigned chunk_stack_size        = 0;
static unsigned chunk_stack_cap         = BUFSIZ;

static void free_hsm_stuff(void) {
    VERBOSE_MSG_LITERAL("HSM_FREE");

    DEBUG_ABORT_IF(!free_map(hsm_statistics))
    NDEBUG_EXECUTE(free_map(hsm_statistics))
    for (ChunkSet* hsm_set = set + HSM_SET_LAST; hsm_set >= set; hsm_set--)
        DEBUG_ABORT_IF(!free_cset(hsm_set))
        NDEBUG_EXECUTE(free_cset(hsm_set))

    while (chunk_stack_cap--)
        free_chunk(chunk_stack + chunk_stack_cap);

    free(chunk_stack);
}

void event_startDocument_hsm(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("HSM_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(hsm_statistics, ENTRY_COUNT_GUESS))

        for (ChunkSet* hsm_set = set + HSM_SET_LAST; hsm_set >= set; hsm_set--)
            DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_cset(hsm_set, CHUNK_SET_RECOMMENDED_PARAMETERS))

        chunk_stack = calloc(chunk_stack_cap, sizeof(Chunk));
        DEBUG_ERROR_IF(chunk_stack == NULL)

        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_chunk(chunk_stack + chunk_stack_size++, BUFSIZ, 1))

        DEBUG_ERROR_IF(atexit(free_hsm_stuff) != 0)
        NDEBUG_EXECUTE(atexit(free_hsm_stuff))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(hsm_statistics))

        for (ChunkSet* hsm_set = set + HSM_SET_LAST; hsm_set >= set; hsm_set--)
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_cset(hsm_set))

        while (chunk_stack_size)
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(chunk_stack + --chunk_stack_size))

        chunk_stack_size = 1;
    }

    hsm_read_state    = HSM_READ_STATE_WAITING;
    nu1_overall       = 0U;
    nu2_overall       = 0U;
    n1_overall        = 0U;
    n2_overall        = 0U;
    nu_overall        = 0U;
    n_overall         = 0U;
    v_overall         = 0.0f;
    d_overall         = 0.0f;
    e_overall         = 0.0f;
}

void event_endDocument_hsm(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("HSM_END => document");

    nu1_overall = getKeyCount_cset(set + HSM_OPERATORS_OVERALL);
    nu2_overall = getKeyCount_cset(set + HSM_OPERANDS_OVERALL);
    nu_overall  = nu1_overall + nu2_overall;
    n_overall   = n1_overall + n2_overall;
    v_overall   = (float)n_overall * log2f((float)nu_overall);
    d_overall   = (.5f * (float)nu1_overall * (float)n2_overall) / (float)(nu2_overall);
    e_overall   = v_overall * d_overall;
    b_overall   = v_overall / 3000.0f;
    t_overall   = e_overall / (18.0f * 3600.0f * 8.0f);

    uint32_t key_id = add_chunk(strings, "HSM-V", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(v_overall)))

    key_id = add_chunk(strings, "HSM-D", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(d_overall)))

    key_id = add_chunk(strings, "HSM-E", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(e_overall)))

    key_id = add_chunk(strings, "HSM-B", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(b_overall)))

    key_id = add_chunk(strings, "HSM-T", 5);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(t_overall)))
}

void event_startUnit_hsm(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("HSM_START => unit");

    flush_cset(set + HSM_OPERATORS_UNIT);
    flush_cset(set + HSM_OPERANDS_UNIT);
    nu1_unit = 0U;
    nu2_unit = 0U;
    n1_unit  = 0U;
    n2_unit  = 0U;
    nu_unit  = 0U;
    n_unit   = 0U;
    v_unit   = 0.0f;
    d_unit   = 0.0f;
    e_unit   = 0.0f;
}

void event_endUnit_hsm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    VERBOSE_MSG_VARIADIC("HSM_END => unit (%s)", get_chunk(strings, unit_id));

    va_end(args);

    nu1_unit = getKeyCount_cset(set + HSM_OPERATORS_UNIT);
    nu2_unit = getKeyCount_cset(set + HSM_OPERANDS_UNIT);
    nu_unit  = nu1_unit + nu2_unit;
    n_unit   = n1_unit + nu2_unit;
    v_unit   = (float)n_unit * log2f((float)nu_unit);
    d_unit   = (.5f * (float)nu1_unit * (float)n2_unit) / (float)(nu2_unit);
    e_unit   = v_unit * d_unit;
    b_unit   = v_unit / 3000.0f;
    t_unit   = e_unit / (18.0f * 3600.0f * 8.0f);

    uint32_t key_id = add_chunk(strings, "HSM-V_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(v_unit)))

    key_id = add_chunk(strings, "HSM-D_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(d_unit)))

    key_id = add_chunk(strings, "HSM-E_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(e_unit)))

    key_id = add_chunk(strings, "HSM-B_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(b_unit)))

    key_id = add_chunk(strings, "HSM-T_", 6);
    DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
    DEBUG_ERROR_IF(appendIndex_chunk(strings, unit_id) == NULL)
    NDEBUG_EXECUTE(appendIndex_chunk(strings, unit_id))
    DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(t_unit)))
}

void event_startElement_hsm(struct srcsax_context* context, ...) {
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
        VERBOSE_MSG_LITERAL("HSM_START => function");
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_cset(set + HSM_OPERATORS_FN))
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_cset(set + HSM_OPERANDS_FN))
        nu1_fn = 0U;
        nu2_fn = 0U;
        n1_fn  = 0U;
        n2_fn  = 0U;
        nu_fn  = 0U;
        n_fn   = 0U;
        v_fn   = 0.0f;
        d_fn   = 0.0f;
        e_fn   = 0.0f;
    } else if (STR_EQ_CONST(localname, "expr")) {
        VERBOSE_MSG_LITERAL("HSM_N2++ (operand)");
        n2_overall++;
        n2_unit += (unit_id != 0xFFFFFFFF);
        n2_fn   += (fn_id   != 0xFFFFFFFF);

        RECALLOC_IF_NECESSARY(
            Chunk*, chunk_stack,
            unsigned, chunk_stack_cap, chunk_stack_size,
            {RECALLOC_ERROR;}
        )

        if (!isValid_chunk(chunk_stack + chunk_stack_size))
            DEBUG_ASSERT_NDEBUG_EXECUTE(
                constructEmpty_chunk(chunk_stack + chunk_stack_size, BUFSIZ, 1)
            )

        chunk_stack_size++;
    } else if (STR_EQ_CONST(localname, "operator")) {
        VERBOSE_MSG_LITERAL("HSM_N1++ (operator)");
        n1_overall++;
        n1_unit += (unit_id != 0xFFFFFFFF);
        n1_fn   += (fn_id   != 0xFFFFFFFF);
        hsm_read_state = HSM_READ_STATE_READING_OPERATOR;
    }
}

void event_endElement_hsm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    char const* const localname = va_arg(args, char const*);

    /* prefix and uri */
    REPEAT(2) va_arg (args, char const*);

    uint32_t const unit_id = va_arg(args, uint32_t);
    uint32_t const fn_id   = va_arg(args, uint32_t);

    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        VERBOSE_MSG_VARIADIC("HSM_END => function (%s)", get_chunk(strings, fn_id));
        nu1_fn  = getKeyCount_cset(set + HSM_OPERATORS_FN);
        nu2_fn  = getKeyCount_cset(set + HSM_OPERANDS_FN);
        nu_fn   = nu1_fn + nu2_fn;
        n_fn    = n1_fn + nu2_fn;
        v_fn    = (float)n_fn * log2f((float)nu_fn);
        d_fn    = (.5f * (float)nu1_fn * (float)n2_fn) / (float)(nu2_fn);
        e_fn    = v_fn * d_fn;
        b_fn    = v_fn / 3000.0f;
        t_fn    = e_fn / (18.0f * 3600.0f * 8.0f);

        uint32_t key_id = add_chunk(strings, "HSM-V_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(v_fn)))

        key_id = add_chunk(strings, "HSM-D_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(d_fn)))

        key_id = add_chunk(strings, "HSM-E_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(e_fn)))

        key_id = add_chunk(strings, "HSM-B_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(b_fn)))

        key_id = add_chunk(strings, "HSM-T_", 6);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(appendIndex_chunk(strings, fn_id) == NULL)
        NDEBUG_EXECUTE(appendIndex_chunk(strings, fn_id))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(hsm_statistics, key_id, VAL_FLOAT(t_fn)))
    } else if (STR_EQ_CONST(localname, "expr")) {
        if (chunk_stack_size <= 1) {TERMINATE_ERROR;}

        Chunk* last_chunk = chunk_stack + --chunk_stack_size;
        char const* const op = getLast_chunk(last_chunk);
        DEBUG_ERROR_IF(op == NULL)

        uint64_t const op_len = strlenLast_chunk(last_chunk);
        DEBUG_ERROR_IF(op_len == 0)

        VERBOSE_MSG_VARIADIC("HSM_OPERAND => %.*s", (int)op_len, op);

        DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERANDS_OVERALL, op, op_len) == 0xFFFFFFFF)
        NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERANDS_OVERALL, op, op_len))

        if (unit_id != 0xFFFFFFFF) {
            DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERANDS_UNIT, op, op_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERANDS_UNIT, op, op_len))
        }
        if (fn_id != 0xFFFFFFFF) {
            DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERANDS_FN, op, op_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERANDS_FN, op, op_len))
        }

        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(last_chunk))
    } else if (STR_EQ_CONST(localname, "operator")) {
        char const* const op = getLast_chunk(chunk_stack);
        DEBUG_ERROR_IF(op == NULL)

        uint64_t const op_len = strlenLast_chunk(chunk_stack);
        DEBUG_ERROR_IF(op_len == 0)

        VERBOSE_MSG_VARIADIC("HSM_OPERATOR => %.*s", (int)op_len, op);

        DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERATORS_OVERALL, op, op_len) == 0xFFFFFFFF)
        NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERATORS_OVERALL, op, op_len))

        if (unit_id != 0xFFFFFFFF) {
            DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERATORS_UNIT, op, op_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERATORS_UNIT, op, op_len))
        }
        if (fn_id != 0xFFFFFFFF) {
            DEBUG_ERROR_IF(addKey_cset(set + HSM_OPERATORS_FN, op, op_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(set + HSM_OPERATORS_FN, op, op_len))
        }

        hsm_read_state = HSM_READ_STATE_WAITING;
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(chunk_stack))
    }
}

void event_charactersUnit_hsm(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const ch = va_arg(args, char*);
    uint64_t const len   = va_arg(args, uint64_t);
    va_end(args);

    if (hsm_read_state == HSM_READ_STATE_READING_OPERATOR) {
        DEBUG_ERROR_IF(append_chunk(chunk_stack, ch, len) == NULL)
        NDEBUG_EXECUTE(append_chunk(chunk_stack, ch, len))
    }

    for (
        Chunk* operand_chunk = chunk_stack + chunk_stack_size - 1;
        operand_chunk > chunk_stack;
        operand_chunk--
    ) {
        DEBUG_ERROR_IF(append_chunk(operand_chunk, ch, len) == NULL)
        NDEBUG_EXECUTE(append_chunk(operand_chunk, ch, len))
    }
}

Map const* report_hsm(void) {
    VERBOSE_MSG_LITERAL("HSM_REPORT");
    return isValid_map(hsm_statistics) ? hsm_statistics : NULL;
}

