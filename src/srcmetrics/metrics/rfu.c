/**
 * @file rfu.c
 * @brief Response for Unit
 * @author Yavuz Koroglu
 */
#include <stdarg.h>
#include "srcmetrics.h"
#include "srcmetrics/metrics/rfu.h"
#include "padkit/chunkset.h"
#include "padkit/debug.h"
#include "padkit/graphmatrix.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"

#define ENTRY_COUNT_GUESS (UNIT_COUNT_GUESS + FN_COUNT_GUESS)

static Map rfu_statistics[1]     = { NOT_A_MAP };

static GraphMatrix callGraph[1]  = { NOT_A_GRAPH_MATRIX };
static ChunkSet units[1]         = { NOT_A_CHUNK_SET };
static ChunkSet fns[1]           = { NOT_A_CHUNK_SET };
static GraphMatrix ownerGraph[1] = { NOT_A_GRAPH_MATRIX };

static uint32_t unit_count       = 0;
static uint32_t fn_count         = 0;

static uint32_t unit_id          = 0xFFFFFFFF;
static uint32_t fn_id            = 0xFFFFFFFF;

static unsigned rfu_fn           = 0U;
static unsigned rfu_unit         = 0U;
static unsigned rfu_overall      = 0U;

static Chunk name_chunk[1]       = { NOT_A_CHUNK };

#define RFU_READ_STATE_WAITING_METHOD       0U
#define RFU_READ_STATE_READING_METHOD       1U
#define RFU_READ_STATE_WAITING_METHOD_NAME  2U
#define RFU_READ_STATE_WAITING_CALL_NAME    3U
#define RFU_READ_STATE_READING_METHOD_NAME  4U
#define RFU_READ_STATE_READING_CALL_NAME    5U
#define RFU_READ_STATE_READING_METHOD_TYPE  6U
static unsigned rfu_read_state = 0U;

static void free_rfu_stuff(void) {
    VERBOSE_MSG_LITERAL("RFU_FREE");

    DEBUG_ABORT_IF(!free_map(rfu_statistics))
    NDEBUG_EXECUTE(free_map(rfu_statistics))

    DEBUG_ABORT_IF(!free_gmtx(callGraph))
    NDEBUG_EXECUTE(free_gmtx(callGraph))

    DEBUG_ABORT_IF(!free_cset(units))
    NDEBUG_EXECUTE(free_cset(units))

    DEBUG_ABORT_IF(!free_cset(fns))
    NDEBUG_EXECUTE(free_cset(fns))

    DEBUG_ABORT_IF(!free_gmtx(ownerGraph))
    NDEBUG_EXECUTE(free_gmtx(ownerGraph))

    DEBUG_ABORT_IF(!free_chunk(name_chunk))
    NDEBUG_EXECUTE(free_chunk(name_chunk))
}

static void generateDot(void) {
    DEBUG_ERROR_IF(options.cg_name == NULL)

    uint64_t const cg_name_len = strlen(options.cg_name);
    DEBUG_ERROR_IF(add_chunk(strings, options.cg_name, cg_name_len) == 0xFFFFFFFF)
    NDEBUG_EXECUTE(add_chunk(strings, options.cg_name, cg_name_len))

    char const* const output_name = append_chunk(strings, ".dot", 7);
    DEBUG_ERROR_IF(output_name == NULL)

    VERBOSE_MSG_VARIADIC("RFU_GENERATE_DOT => %s", output_name);

    FILE* const cg = fopen(output_name, "w");
    DEBUG_ERROR_IF(cg == NULL)

    fputs("graph CG {\n"
          "    graph [labelloc=\"t\",label=\"Call Graph\",style=\"filled\",fillcolor=\"#CCCCCC\",rankdir=\"LR\"];\n"
          "    node [shape=\"rectangle\",style=\"rounded,filled\",fillcolor=\"#EEEEEE\"];\n"
          "    edge [dir=\"forward\"];\n", cg);

    uint32_t cluster_id = 0;
    if (!isCGNoExternal()) {
        fprintf(cg, "    subgraph cluster_%d {\n"
                            "        graph [label=\"External\"];\n", cluster_id++);
        for (fn_id = fn_count - 1; fn_id != 0xFFFFFFFF; fn_id--) {
            unit_id = findSink_gmtx(ownerGraph, fn_id, unit_count - 1);
            if (unit_id != 0xFFFFFFFF) continue;
            char const* const node_name = getKey_cset(fns, fn_id);
            fprintf(cg, "        \"%s()\";\n", node_name);
        }
        fputs("    }\n", cg);
    }
    for (unit_id = unit_count - 1; unit_id != 0xFFFFFFFF; unit_id--) {
        char const* const unit_name = getKey_cset(units, unit_id);
        fprintf(cg, "    subgraph cluster_%d {\n"
                            "        graph [label=\"%s\"];\n", cluster_id++, unit_name);
        for (
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_count - 1);
            fn_id != 0xFFFFFFFF;
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_id - 1)
        ) {
            char const* const fn_name = getKey_cset(fns, fn_id);
            fprintf(cg, "        \"%s::%s()\" [label=\"%s()\"];\n", unit_name, fn_name, fn_name);
        }
        fputs("    }\n", cg);
    }
    for (uint32_t source_unit_id = unit_count - 1; source_unit_id != 0xFFFFFFFF; source_unit_id--) {
        char const* const source_unit_name = getKey_cset(units, source_unit_id);
        for (
            uint32_t source_fn_id = findSource_gmtx(ownerGraph, source_unit_id, fn_count - 1);
            source_fn_id != 0xFFFFFFFF;
            source_fn_id = findSource_gmtx(ownerGraph, source_unit_id, source_fn_id - 1)
        ) {
            char const* const source_fn_name = getKey_cset(fns, source_fn_id);
            if (isConnected_gmtx(callGraph, source_fn_id, source_fn_id)) {
                fprintf(cg,
                    "    \"%s::%s()\"--\"%s::%s()\";\n",
                    source_unit_name, source_fn_name,
                    source_unit_name, source_fn_name
                );
            } else {
                for (
                    uint32_t sink_fn_id = findSink_gmtx(callGraph, source_fn_id, fn_count - 1);
                    sink_fn_id != 0xFFFFFFFF;
                    sink_fn_id = findSink_gmtx(callGraph, source_fn_id, sink_fn_id - 1)
                ) {
                    char const* const sink_fn_name = getKey_cset(fns, sink_fn_id);
                    uint32_t sink_unit_id = findSink_gmtx(ownerGraph, sink_fn_id, unit_count - 1);
                    if (sink_unit_id == 0xFFFFFFFF && !isCGNoExternal()) {
                        fprintf(cg,
                            "    \"%s::%s()\"--\"%s()\" [style=\"dashed\"];\n",
                            source_unit_name, source_fn_name,
                            sink_fn_name
                        );
                    } else {
                        while (sink_unit_id != 0xFFFFFFFF) {
                            char const* const sink_unit_name = getKey_cset(units, sink_unit_id);
                            if (source_unit_id == sink_unit_id) {
                                fprintf(cg,
                                    "    \"%s::%s()\"--\"%s::%s()\";\n",
                                    source_unit_name, source_fn_name,
                                    source_unit_name, sink_fn_name
                                );
                            } else {
                                fprintf(cg,
                                    "    \"%s::%s()\"--\"%s::%s()\" [style=\"dashed\"];\n",
                                    source_unit_name, source_fn_name,
                                    sink_unit_name, sink_fn_name
                                );
                            }
                            sink_unit_id = findSink_gmtx(ownerGraph, sink_fn_id, sink_unit_id - 1);
                        }
                    }
                }
            }
        }
    }
    fputs("}", cg);

    DEBUG_ERROR_IF(fclose(cg) == EOF)
    NDEBUG_EXECUTE(fclose(cg))
}

static void generateXml(void) {
    DEBUG_ERROR_IF(options.cg_name == NULL)

    uint64_t const cg_name_len    = strlen(options.cg_name);
    DEBUG_ERROR_IF(add_chunk(strings, options.cg_name, cg_name_len) == 0xFFFFFFFF)
    NDEBUG_EXECUTE(add_chunk(strings, options.cg_name, cg_name_len))

    char const* const output_name = append_chunk(strings, ".xml", 7);
    DEBUG_ERROR_IF(output_name == NULL)

    VERBOSE_MSG_VARIADIC("RFU_GENERATE_XML => %s", output_name);

    FILE* const cg = fopen(output_name, "w");
    DEBUG_ERROR_IF(cg == NULL)

    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<graphml xmlns=\"http:/""/graphml.graphdrawing.org/xmlns\"\n"
          "    xmlns:xsi=\"http:/""/www.w3.org/2001/XMLSchema-instance\"\n"
          "    xsi:schemaLocation=\"http:/""/graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n"
          "    <graph id=\"CG\" edgedefault=\"directed\">\n", cg);

    if (!isCGNoExternal()) {
        for (fn_id = fn_count - 1; fn_id != 0xFFFFFFFF; fn_id--) {
            unit_id = findSink_gmtx(ownerGraph, fn_id, unit_count - 1);
            if (unit_id != 0xFFFFFFFF) continue;
            char const* const node_id = getKey_cset(fns, fn_id);
            fprintf(cg, "        <node id=\"%s()\"/>\n", node_id);
        }
    }
    for (unit_id = unit_count - 1; unit_id != 0xFFFFFFFF; unit_id--) {
        char const* const unit_name = getKey_cset(units, unit_id);
        for (
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_count - 1);
            fn_id != 0xFFFFFFFF;
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_id - 1)
        ) {
            char const* const fn_name = getKey_cset(fns, fn_id);
            fprintf(cg, "        <node id=\"%s::%s()\">\n", unit_name, fn_name);
        }
    }
    for (uint32_t source_unit_id = unit_count - 1; source_unit_id != 0xFFFFFFFF; source_unit_id--) {
        char const* const source_unit_name = getKey_cset(units, source_unit_id);
        for (
            uint32_t source_fn_id = findSource_gmtx(ownerGraph, source_unit_id, fn_count - 1);
            source_fn_id != 0xFFFFFFFF;
            source_fn_id = findSource_gmtx(ownerGraph, source_unit_id, source_fn_id - 1)
        ) {
            char const* const source_fn_name = getKey_cset(fns, source_fn_id);
            if (isConnected_gmtx(callGraph, source_fn_id, source_fn_id)) {
                fprintf(cg,
                    "        <edge id=\"%s::%s()--%s::%s()\" source=\"%s::%s()\" sink=\"%s::%s()\"/>\n",
                    source_unit_name, source_fn_name,
                    source_unit_name, source_fn_name,
                    source_unit_name, source_fn_name,
                    source_unit_name, source_fn_name
                );
            } else {
                for (
                    uint32_t sink_fn_id = findSink_gmtx(callGraph, source_fn_id, fn_count - 1);
                    sink_fn_id != 0xFFFFFFFF;
                    sink_fn_id = findSink_gmtx(callGraph, source_fn_id, sink_fn_id - 1)
                ) {
                    char const* const sink_fn_name = getKey_cset(fns, sink_fn_id);
                    uint32_t sink_unit_id = findSink_gmtx(ownerGraph, sink_fn_id, unit_count - 1);
                    if (sink_unit_id == 0xFFFFFFFF && !isCGNoExternal()) {
                        fprintf(cg,
                            "        <edge id=\"%s::%s()--%s()\" source=\"%s::%s()\" sink=\"%s()\"/>\n",
                            source_unit_name, source_fn_name,
                            sink_fn_name,
                            source_unit_name, source_fn_name,
                            sink_fn_name
                        );
                    } else {
                        while (sink_unit_id != 0xFFFFFFFF) {
                            char const* const sink_unit_name = getKey_cset(units, sink_unit_id);
                            if (source_unit_id == sink_unit_id) {
                                fprintf(cg,
                                    "        <edge id=\"%s::%s()--%s::%s()\" source=\"%s::%s()\" sink=\"%s::%s()\"/>\n",
                                    source_unit_name, source_fn_name,
                                    source_unit_name, sink_fn_name,
                                    source_unit_name, source_fn_name,
                                    source_unit_name, sink_fn_name
                                );
                            } else {
                                fprintf(cg,
                                    "        <edge id=\"%s::%s()--%s::%s()\" source=\"%s::%s()\" sink=\"%s::%s()\" style=\"dashed\"/>\n",
                                    source_unit_name, source_fn_name,
                                    sink_unit_name, sink_fn_name,
                                    source_unit_name, source_fn_name,
                                    sink_unit_name, sink_fn_name
                                );
                            }
                            sink_unit_id = findSink_gmtx(ownerGraph, sink_fn_id, sink_unit_id - 1);
                        }
                    }
                }
            }
        }
    }
    fputs("    </graph>\n", cg);
    fputs("</graphml>", cg);

    DEBUG_ERROR_IF(fclose(cg) == EOF)
}

void event_startDocument_rfu(struct srcsax_context* context, ...) {
    static bool first_time_execution = 1;

    VERBOSE_MSG_LITERAL("RFU_START => document");

    if (first_time_execution) {
        first_time_execution = 0;

        if (!isRFUQuiet()) {
            DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(rfu_statistics, ENTRY_COUNT_GUESS))
            DEBUG_ASSERT_NDEBUG_EXECUTE(construct_gmtx(callGraph, FN_COUNT_GUESS, FN_COUNT_GUESS))
            DEBUG_ASSERT_NDEBUG_EXECUTE(construct_gmtx(ownerGraph, FN_COUNT_GUESS, UNIT_COUNT_GUESS))
            DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_cset(
                units,
                CHUNK_RECOMMENDED_INITIAL_CAP,
                UNIT_COUNT_GUESS,
                CHUNK_SET_RECOMMENDED_LOAD_PERCENT
            ))
            DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_cset(
                fns,
                CHUNK_RECOMMENDED_INITIAL_CAP,
                FN_COUNT_GUESS,
                CHUNK_SET_RECOMMENDED_LOAD_PERCENT
            ))
            DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_chunk(name_chunk, BUFSIZ, 1))

            DEBUG_ERROR_IF(atexit(free_rfu_stuff) != 0)
            NDEBUG_EXECUTE(atexit(free_rfu_stuff))
        }
    } else {
        if (!isRFUQuiet()) {
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(rfu_statistics))
            DEBUG_ASSERT_NDEBUG_EXECUTE(disconnectAll_gmtx(callGraph))
            DEBUG_ASSERT_NDEBUG_EXECUTE(disconnectAll_gmtx(ownerGraph))
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_cset(units))
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_cset(fns))
            DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(name_chunk))
        }
    }

    unit_count     = 0;
    unit_id        = 0xFFFFFFFF;
    fn_count       = 0;
    rfu_overall    = 0U;
}

void event_endDocument_rfu(struct srcsax_context* context, ...) {
    VERBOSE_MSG_LITERAL("RFU_END => document");

    if (isCGEnabled()) {
        if (isDotEnabled()) generateDot();
        if (isXmlEnabled()) generateXml();
    }

    if (!isRFUQuiet()) {
        rfu_overall = fn_count;
        uint32_t const key_id = add_chunk(strings, "RFU", 3);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(rfu_statistics, key_id, VAL_UNSIGNED(rfu_overall)))
    }
}

void event_startUnit_rfu(struct srcsax_context* context, ...) {
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

    unit_id = va_arg(args, uint32_t);

    va_end(args);

    rfu_unit                     = 0U;
    char const* const unit_name  = get_chunk(strings, unit_id);
    uint64_t const unit_name_len = strlen_chunk(strings, unit_id);
    unit_id = addKey_cset(units, unit_name, unit_name_len);
    DEBUG_ERROR_IF(unit_id == 0xFFFFFFFF)
    VERBOSE_MSG_VARIADIC("RFU_START => unit (%s)", unit_name);

    unit_count = getKeyCount_cset(units);
}

void event_endUnit_rfu(struct srcsax_context* context, ...) {
    VERBOSE_MSG_VARIADIC("RFU_END => unit (%s)", getKey_cset(units, unit_id));
    if (!isRFUQuiet()) {
        ChunkSet unique_calls[1];
        DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_cset(unique_calls, CHUNK_SET_RECOMMENDED_PARAMETERS))

        char const* const unit_name = getKey_cset(units, unit_id);
        uint64_t const unit_len     = strlen_cset(units, unit_id);
        for (
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_count - 1);
            fn_id != 0xFFFFFFFF;
            fn_id = findSource_gmtx(ownerGraph, unit_id, fn_id - 1)
        ) {
            /* Always, all the unit's functions count. */
            char const* const fn_name     = getKey_cset(fns, fn_id);
            uint64_t const fn_name_len    = strlen_cset(fns, fn_id);
            DEBUG_ERROR_IF(addKey_cset(unique_calls, fn_name, fn_name_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(unique_calls, fn_name, fn_name_len))

            /* Always, all the direct calls count. */
            for (
                uint32_t sink_fn_id = findSink_gmtx(callGraph, fn_id, fn_count - 1);
                sink_fn_id != 0xFFFFFFFF;
                sink_fn_id = findSink_gmtx(callGraph, fn_id, sink_fn_id - 1)
            ) {
                char const* const sink_fn  = getKey_cset(fns, sink_fn_id);
                uint64_t const sink_fn_len = strlen_cset(fns, sink_fn_id);
                DEBUG_ERROR_IF(addKey_cset(unique_calls, sink_fn, sink_fn_len) == 0xFFFFFFFF)
                NDEBUG_EXECUTE(addKey_cset(unique_calls, sink_fn, sink_fn_len))
            }
        }
        if (!isRFUSimple()) {
            /* RFU_Transitive */
            for (
                uint32_t unique_call_id = 0;
                unique_call_id < getKeyCount_cset(unique_calls);
                unique_call_id++
            ) {
                char const* const unique_call_name = getKey_cset(unique_calls, unique_call_id);
                uint64_t const unique_call_len     = strlen_cset(unique_calls, unique_call_id);
                uint32_t const source_fn_id        = getKeyId_cset(fns, unique_call_name, unique_call_len);
                for (
                    uint32_t sink_fn_id = findSink_gmtx(callGraph, source_fn_id, fn_count - 1);
                    sink_fn_id != 0xFFFFFFFF;
                    sink_fn_id = findSink_gmtx(callGraph, source_fn_id, sink_fn_id - 1)
                ) {
                    char const* const sink_key  = getKey_cset(fns, sink_fn_id);
                    uint64_t const sink_key_len = strlen_cset(fns, sink_fn_id);
                    DEBUG_ERROR_IF(addKey_cset(unique_calls, sink_key, sink_key_len) == 0xFFFFFFFF)
                    NDEBUG_EXECUTE(addKey_cset(unique_calls, sink_key, sink_key_len))
                }
            }
        }
        rfu_unit = getKeyCount_cset(unique_calls);
        DEBUG_ASSERT_NDEBUG_EXECUTE(free_cset(unique_calls))

        uint32_t key_id = add_chunk(strings, "RFU_", 4);
        DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(append_chunk(strings, unit_name, unit_len) == NULL)
        NDEBUG_EXECUTE(append_chunk(strings, unit_name, unit_len))
        DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(rfu_statistics, key_id, VAL_UNSIGNED(rfu_unit)))
    }
}

void event_startElement_rfu(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    switch (rfu_read_state) {
        case RFU_READ_STATE_WAITING_METHOD:
            if (STR_EQ_CONST(localname, "function"))  { rfu_fn = 0U; rfu_read_state = RFU_READ_STATE_READING_METHOD; }
            else if (STR_EQ_CONST(localname, "call")) { rfu_read_state = RFU_READ_STATE_WAITING_CALL_NAME; }
            break;
        case RFU_READ_STATE_READING_METHOD:
            if (STR_EQ_CONST(localname, "type")) { rfu_read_state = RFU_READ_STATE_READING_METHOD_TYPE; }
            break;
        case RFU_READ_STATE_WAITING_METHOD_NAME:
            if (STR_EQ_CONST(localname, "name")) { rfu_read_state = RFU_READ_STATE_READING_METHOD_NAME; }
            break;
        case RFU_READ_STATE_WAITING_CALL_NAME:
            if (STR_EQ_CONST(localname, "name")) { rfu_read_state = RFU_READ_STATE_READING_CALL_NAME; }
    }
}

void event_endElement_rfu(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const localname = va_arg(args, char const*);
    va_end(args);

    if (STR_EQ_CONST(localname, "function")) {
        if (!isRFUQuiet()) {
            char const* const unit_name = getKey_cset(units, unit_id);
            char const* const fn_name   = getKey_cset(fns, fn_id);
            uint64_t const unit_len     = strlen_cset(units, unit_id);
            uint64_t const fn_len       = strlen_cset(fns, fn_id);
            ChunkSet unique_calls[1];
            DEBUG_ASSERT_NDEBUG_EXECUTE(
                constructEmpty_cset(unique_calls, CHUNK_SET_RECOMMENDED_PARAMETERS)
            )

            DEBUG_ERROR_IF(addKey_cset(unique_calls, fn_name, fn_len) == 0xFFFFFFFF)
            NDEBUG_EXECUTE(addKey_cset(unique_calls, fn_name, fn_len))

            for (
                uint32_t sink_fn_id = findSink_gmtx(callGraph, fn_id, fn_count - 1);
                sink_fn_id != 0xFFFFFFFF;
                sink_fn_id = findSink_gmtx(callGraph, fn_id, sink_fn_id - 1)
            ) {
                char const* const sink_fn_name  = getKey_cset(fns, sink_fn_id);
                uint64_t const sink_fn_name_len = strlen_cset(fns, sink_fn_id);
                DEBUG_ERROR_IF(addKey_cset(unique_calls, sink_fn_name, sink_fn_name_len) == 0xFFFFFFFF)
                NDEBUG_EXECUTE(addKey_cset(unique_calls, sink_fn_name, sink_fn_name_len))
            }
            if (!isRFUSimple()) {
                /* RFU_Transitive */
                for (
                    uint32_t unique_call_id = 0;
                    unique_call_id < getKeyCount_cset(unique_calls);
                    unique_call_id++
                ) {
                    char const* const unique_call_name = getKey_cset(unique_calls, unique_call_id);
                    uint64_t const unique_call_len     = strlen_cset(unique_calls, unique_call_id);
                    uint32_t const source_fn_id        = getKeyId_cset(fns, unique_call_name, unique_call_len);
                    for (
                        uint32_t sink_fn_id = findSink_gmtx(callGraph, source_fn_id, fn_count - 1);
                        sink_fn_id != 0xFFFFFFFF;
                        sink_fn_id = findSink_gmtx(callGraph, source_fn_id, sink_fn_id - 1)
                    ) {
                        char const* const sink_fn_name  = getKey_cset(fns, sink_fn_id);
                        uint64_t const sink_fn_name_len = strlen_cset(fns, sink_fn_id);
                        DEBUG_ERROR_IF(addKey_cset(unique_calls, sink_fn_name, sink_fn_name_len) == 0xFFFFFFFF)
                        NDEBUG_EXECUTE(addKey_cset(unique_calls, sink_fn_name, sink_fn_name_len))
                    }
                }
            }
            rfu_fn = getKeyCount_cset(unique_calls);
            DEBUG_ASSERT_NDEBUG_EXECUTE(free_cset(unique_calls))

            /* Record RFU for this function */
            uint32_t const key_id = add_chunk(strings, "RFU_", 4);
            DEBUG_ERROR_IF(key_id == 0xFFFFFFFF)
            DEBUG_ERROR_IF(append_chunk(strings, unit_name, unit_len) == NULL)
            NDEBUG_EXECUTE(append_chunk(strings, unit_name, unit_len))
            DEBUG_ERROR_IF(append_chunk(strings, "::", 2) == NULL)
            NDEBUG_EXECUTE(append_chunk(strings, "::", 2))
            DEBUG_ERROR_IF(append_chunk(strings, fn_name, fn_len) == NULL)
            NDEBUG_EXECUTE(append_chunk(strings, fn_name, fn_len))
            DEBUG_ERROR_IF(append_chunk(strings, "()", 2) == NULL)
            NDEBUG_EXECUTE(append_chunk(strings, "()", 2))
            DEBUG_ASSERT_NDEBUG_EXECUTE(insert_map(rfu_statistics, key_id, VAL_UNSIGNED(rfu_fn)))
        }
        fn_id = 0xFFFFFFFF;
    } else if (rfu_read_state == RFU_READ_STATE_READING_METHOD_TYPE && STR_EQ_CONST(localname, "type")) {
        rfu_read_state = RFU_READ_STATE_WAITING_METHOD_NAME;
    } else if (rfu_read_state == RFU_READ_STATE_READING_METHOD_NAME && STR_EQ_CONST(localname, "name")) {
        char const* const fn_name = getLast_chunk(name_chunk);
        DEBUG_ERROR_IF(fn_name == NULL)

        uint64_t const fn_len = strlenLast_chunk(name_chunk);
        DEBUG_ERROR_IF(fn_len == 0)

        fn_id = addKey_cset(fns, fn_name, fn_len);
        DEBUG_ERROR_IF(fn_id == 0xFFFFFFFF)
        DEBUG_ASSERT_NDEBUG_EXECUTE(connect_gmtx(ownerGraph, fn_id, unit_id))

        fn_count       = getKeyCount_cset(fns);
        rfu_read_state = RFU_READ_STATE_WAITING_METHOD;
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(name_chunk))
    } else if (rfu_read_state == RFU_READ_STATE_READING_CALL_NAME && STR_EQ_CONST(localname, "name")) {
        char const* const sink_fn_name = getLast_chunk(name_chunk);
        DEBUG_ERROR_IF(sink_fn_name == NULL)

        uint64_t const sink_fn_len = strlenLast_chunk(name_chunk);
        DEBUG_ERROR_IF(sink_fn_len == 0)

        uint32_t const sink_fn_id = addKey_cset(fns, sink_fn_name, sink_fn_len);
        DEBUG_ERROR_IF(sink_fn_id == 0xFFFFFFFF)
        DEBUG_ASSERT_NDEBUG_EXECUTE(connect_gmtx(callGraph, fn_id, sink_fn_id))

        fn_count       = getKeyCount_cset(fns);
        rfu_read_state = RFU_READ_STATE_WAITING_METHOD;
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(name_chunk))
    }
}

void event_charactersUnit_rfu(struct srcsax_context* context, ...) {
    va_list args;

    va_start(args, context);
    char const* const ch = va_arg(args, char*);
    uint64_t const len   = va_arg(args, uint64_t);
    va_end(args);

    switch (rfu_read_state) {
        case RFU_READ_STATE_READING_METHOD_NAME:
        case RFU_READ_STATE_READING_CALL_NAME:
            DEBUG_ERROR_IF(append_chunk(name_chunk, ch, len) == NULL)
            NDEBUG_EXECUTE(append_chunk(name_chunk, ch, len))
    }
}

Map const* report_rfu(void) {
    if (isRFUQuiet()) {
        return NULL;
    } else {
        VERBOSE_MSG_LITERAL("RFU_REPORT");
        return isValid_map(rfu_statistics) ? rfu_statistics : NULL;
    }
}

