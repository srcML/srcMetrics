/**
 * @file cc.c
 * @brief Cyclomatic Complexity
 * @author Yavuz Koroglu
 */
#include "languages/c.h"
#include "srcmetrics.h"
#include "srcmetrics/metrics/cc.h"
#include "padkit/debug.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

static Map cc_statistics[1] = { NOT_A_MAP };
static CParse cparse[1]     = { NOT_A_CPARSE };

static void free_cc_stuff(void) {
    VERBOSE_MSG_LITERAL("CC_FREE");

    DEBUG_ABORT_IF(!free_map(cc_statistics))
    NDEBUG_EXECUTE(free_map(cc_statistics))
    free_cparse(cparse);
}

void event_startDocument_cc(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("CC_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        DEBUG_ASSERT_NDEBUG_EXECUTE(
            constructEmpty_map(cc_statistics, ENTRY_COUNT_GUESS)
        )

        constructEmpty_cparse(
            cparse,
            CPARSE_RECOMMENDED_CHUNK_SIZE,
            CPARSE_RECOMMENDED_CHUNK_ITEM_COUNT,
            UNIT_COUNT_GUESS,
            FN_COUNT_GUESS,
            CPARSE_RECOMMENDED_LOAD_PERCENT,
            CPARSE_RECOMMENDED_INITIAL_STACK_CAP
        );

        DEBUG_ERROR_IF(atexit(free_cc_stuff) != 0)
        NDEBUG_EXECUTE(atexit(free_cc_stuff))
    } else {
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(cc_statistics))
        flush_cparse(cparse);
    }
}

void event_endDocument_cc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("CC_END => document");

    if (isCFGEnabled()) {
        if (isDotEnabled()) {
            uint64_t const cfg_name_len = strlen(options.cfg_name);
            DEBUG_ERROR_IF(add_chunk(strings, options.cfg_name, cfg_name_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(add_chunk(strings, options.cfg_name, cfg_name_len))

            char const* filename = append_chunk(strings, ".dot", 4);
            DEBUG_ERROR_IF(filename == NULL)

            generateDot_cparse(cparse, filename, 0);
        }
    }
}

void event_startUnit_cc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);

    /* localname, prefix, uri */
    REPEAT(3) va_arg(args, char const*);

    /* num_namespaces */
    va_arg(args, int);

    /* namespaces */
    va_arg(args, void const*);

    /* num_attributes */
    va_arg(args, int);

    /* attributes */
    va_arg(args, void const*);

    uint32_t const unit_id = va_arg(args, uint32_t);

    va_end(args);

    char const* const unit_name = get_chunk(strings, unit_id);
    DEBUG_ERROR_IF(unit_name == NULL)

    uint64_t const unit_len = strlen_chunk(strings, unit_id);
    DEBUG_ERROR_IF(unit_len == 0xFFFFFFFFFFFFFFFF)

    VERBOSE_MSG_VARIADIC("CC_START_UNIT (%s)", unit_name);
    startUnit_cparse(cparse, unit_name, unit_len);
}

void event_endUnit_cc(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("CC_END_UNIT");

    endUnit_cparse(cparse);
}

void event_startElement_cc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);

    /* prefix, uri. */
    REPEAT(2) va_arg(args, char const*);

    /* num_namespaces */
    va_arg(args, int);

    /* namespaces */
    va_arg(args, void const*);

    int const num_attributes = va_arg(args, int);

    struct srcsax_attribute const* const attributes
        = va_arg(args, struct srcsax_attribute const*);

    va_end(args);

    for (
        struct srcsax_attribute const* attribute = attributes + num_attributes - 1;
        attribute >= attributes;
        attribute--)
    {
        if (
            STR_EQ_CONST(attribute->localname, "type") &&
            STR_EQ_CONST(attribute->value, "elseif")
        ) {
            VERBOSE_MSG_LITERAL("CC_START_ELEMENT => elseif");
            start_cparse(cparse, attribute->value);
            return;
        }
    }

    VERBOSE_MSG_VARIADIC("CC_START_ELEMENT => %s", localname);
    start_cparse(cparse, localname);
}

void event_endElement_cc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    VERBOSE_MSG_VARIADIC("CC_END_ELEMENT => %s", localname);
    end_cparse(cparse, localname);
}

void event_charactersUnit_cc(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const ch = va_arg(args, char const*);
    uint64_t const len   = va_arg(args, uint64_t);
    va_end(args);

    appendIfPossible_cparse(cparse, ch, len);
}

Map const* report_cc(void) {
    VERBOSE_MSG_LITERAL("CC_REPORT");
    return isValid_map(cc_statistics) ? cc_statistics : NULL;
}

