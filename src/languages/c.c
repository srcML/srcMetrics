#include <stdarg.h>
#include <stdlib.h>
#include "languages/c.h"
#include "srcmetrics.h"
#include "padkit/debug.h"
#include "padkit/reallocate.h"
#include "padkit/repeat.h"
#include "padkit/streq.h"
#include "padkit/timestamp.h"

static char const* element_tags[] = C_ELEMENT_TAGS;

static uint32_t interpret_cparse(CParse* const cparse, uint8_t const interpretation) {
    static char const* const c_interpretation_names[] = C_INTERPRETATION_NAMES;

    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ASSERT(C_IS_VALID_INTERPRETATION(interpretation))

    REALLOC_IF_NECESSARY(
        uint8_t, cparse->interpretations,
        uint32_t, cparse->interpretations_cap, cparse->interpretations_size,
        {REALLOC_ERROR;}
    )

    VERBOSE_MSG_VARIADIC("CPARSE_INTERPRET => %s", c_interpretation_names[interpretation]);

    cparse->interpretations[cparse->interpretations_size] = interpretation;

    return cparse->interpretations_size++;
}

static void parse_cparse(
    CParse* const cparse, char const* const str,
    uint64_t const len, uint8_t const interpretation
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(str == NULL)
    DEBUG_ASSERT(C_IS_VALID_INTERPRETATION(interpretation))

    #ifndef NDEBUG
        uint32_t const id = add_chunk(cparse->chunks + CPARSE_CHUNK_PARSE, str, len);
        DEBUG_ERROR_IF(id == 0xFFFFFFFF)
        DEBUG_ERROR_IF(interpret_cparse(cparse, interpretation) != id)
    #else
        add_chunk(cparse->chunks + CPARSE_CHUNK_PARSE, str, len);
        interpret_cparse(cparse, interpretation);
    #endif
}

static uint32_t peekStack_cparse(CParse const* const cparse, unsigned const stack_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(stack_id > CPARSE_STACK_LAST)
    if (cparse->stack_size[stack_id] == 0) return 0xFFFFFFFF;
    return cparse->stack[stack_id][cparse->stack_size[stack_id] - 1];
}

static uint32_t popStack_cparse(CParse* const cparse, unsigned const stack_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(stack_id > CPARSE_STACK_LAST)
    if (cparse->stack_size[stack_id] == 0) return 0xFFFFFFFF;
    return cparse->stack[stack_id][--cparse->stack_size[stack_id]];
}

static void pushStack_cparse(
    CParse* const cparse, unsigned const stack_id, uint32_t const element
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(stack_id > CPARSE_STACK_LAST)

    REALLOC_IF_NECESSARY(
        uint32_t, cparse->stack[stack_id],
        uint32_t, cparse->stack_cap[stack_id], cparse->stack_size[stack_id],
        {REALLOC_ERROR;}
    )

    cparse->stack[stack_id][cparse->stack_size[stack_id]++] = element;
}

static void start_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    if (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS) == C_BLOCK_CONTENT)
        return;

    parse_cparse(cparse, "", 0, C_STMT);
}
static void start_block_content_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    parse_cparse(cparse, "", 0, C_SPURIOUS);

    uint32_t const currentStmt_id = CPARSE_LAST_CHUNK_ID;
    DEBUG_ERROR_IF(currentStmt_id == 0xFFFFFFFF)

    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_FUNCTION:
            if (peekStack_cparse(cparse, CPARSE_STACK_INCOMING) != 0xFFFFFFFF) {
                VERBOSE_MSG_LITERAL("CPARSE_DANGLING_CONTROL_FLOW_FROM_PREVIOUS_FN");
                TERMINATE_ERROR;
            }
            if (cparse->tracked_id[CPARSE_CURRENT_FN_FIRST_STMT] != 0xFFFFFFFF) {
                VERBOSE_MSG_LITERAL("CPARSE_NESTED_FUNCTION");
                TERMINATE_ERROR;
            }
            cparse->tracked_id[CPARSE_CURRENT_FN_FIRST_STMT] = currentStmt_id;
    }

    uint32_t const incomingStmt_id = popStack_cparse(cparse, CPARSE_STACK_INCOMING);
    if (incomingStmt_id != 0xFFFFFFFF) {
        Map* const edges = cparse->maps + CPARSE_MAP_EDGES;
        DEBUG_ASSERT_NDEBUG_EXECUTE(
            insert_map(edges, incomingStmt_id, VAL_UNSIGNED(currentStmt_id))
        )
    }

    pushStack_cparse(cparse, CPARSE_STACK_INCOMING, currentStmt_id);
    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_BLOCK_CONTENT);
}
static void start_break_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    if (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_BLOCK_CONTENT)
        return;

    parse_cparse(cparse, "", 0, C_SPURIOUS);

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_BREAK);
}
static void start_call_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    if (cparse->interpretations[cparse->interpretations_size - 1] != C_STMT)
        return;

    Chunk* const call_stack = cparse->chunks + CPARSE_CHUNK_CALLS;
    uint32_t const call_id  = add_chunk(call_stack, "", 0);
    DEBUG_ERROR_IF(call_id == 0xFFFFFFFF)

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_CALL);
}
static void start_case_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_CASE);
}
static void start_condition_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_CONDITION);
}
static void start_continue_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_CONTINUE);
}
static void start_for_ctrl_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_CONTROL);
}
static void start_decl_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    start_stmt_cparse(cparse);

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_DECL_STMT);
}
static void start_default_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_DEFAULT);
}
static void start_do_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_DO);
}
static void start_else_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_ELSE);
}
static void start_elseif_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_ELSEIF);
}
static void start_empty_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    start_stmt_cparse(cparse);

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_EMPTY_STMT);
}
static void start_expr_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    start_stmt_cparse(cparse);

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_EXPR_STMT);
}
static void start_for_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_FOR);
}
static void start_fn_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    if (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != 0xFFFFFFFF) {
        VERBOSE_MSG_LITERAL("CPARSE_NESTED_FUNCTION");
        TERMINATE_ERROR;
    }
    if (cparse->tracked_id[CPARSE_CURRENT_FN] != 0xFFFFFFFF) {
        VERBOSE_MSG_LITERAL("CPARSE_NESTED_FUNCTION");
        TERMINATE_ERROR;
    }

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_FUNCTION);
}
static void start_goto_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_GOTO);
}
static void start_if_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_IF);
}
static void start_if_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_IF_STMT);
}
static void start_incr_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_INCR);
}
static void start_init_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_INIT);
}
static void start_label_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_LABEL);
}
static void start_name_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_FUNCTION:
            if (cparse->tracked_id[CPARSE_CURRENT_FN] != 0xFFFFFFFF) {
                VERBOSE_MSG_LITERAL("CPARSE_NESTED_FUNCTION");
                TERMINATE_ERROR;
            }
            parse_cparse(cparse, "", 0, C_FN_NAME);
            cparse->tracked_id[CPARSE_CURRENT_FN] = CPARSE_LAST_CHUNK_ID;
            DEBUG_ERROR_IF(cparse->tracked_id[CPARSE_CURRENT_FN] == 0xFFFFFFFF)
    }

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_NAME);
}
static void start_params_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_FUNCTION:
            parse_cparse(cparse, "", 0, C_FN_PARAMS);
    }

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_PARAM_LIST);
}
static void start_return_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    start_stmt_cparse(cparse);

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_RETURN);
}
static void start_switch_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_SWITCH);
}
static void start_type_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_FUNCTION:
            parse_cparse(cparse, "", 0, C_FN_TYPE);
    }

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_TYPE);
}
static void start_while_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    pushStack_cparse(cparse, CPARSE_STACK_ELEMENTS, C_WHILE);
}

static void end_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))
}
static void end_block_content_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_BLOCK_CONTENT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_break_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_BREAK)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_call_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_CALL)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_case_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_CASE)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_condition_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_CONDITION)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_continue_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_CONTINUE)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_for_ctrl_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_CONTROL)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_decl_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    end_stmt_cparse(cparse);

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_DECL_STMT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_default_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_DEFAULT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_do_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_DO)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_else_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_ELSE)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_elseif_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_ELSEIF)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_empty_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    end_stmt_cparse(cparse);

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_EMPTY_STMT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_expr_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    end_stmt_cparse(cparse);

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_EXPR_STMT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_for_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_FOR)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_fn_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    unsigned c_element_id = peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS);
    if (c_element_id != 0xFFFFFFFF) {
        VERBOSE_MSG_VARIADIC("CPARSE_FN_END_BEFORE </%s>", element_tags[c_element_id]);
        TERMINATE_ERROR;
    }
    cparse->tracked_id[CPARSE_CURRENT_FN]            = 0xFFFFFFFF;
    cparse->tracked_id[CPARSE_CURRENT_FN_FIRST_STMT] = 0xFFFFFFFF;

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_FUNCTION)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_goto_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_GOTO)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_if_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_IF)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_if_stmt_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_IF_STMT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_incr_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_INCR)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_init_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_INIT)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_label_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_LABEL)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_name_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_FUNCTION:
            {
                Chunk* const parse_chunk = cparse->chunks + CPARSE_CHUNK_PARSE;
                ChunkTable* const units  = cparse->tables + CPARSE_TABLE_UNITS;
                ChunkTable* const fns    = cparse->tables + CPARSE_TABLE_FNS;
                uint32_t const unit_id   = cparse->tracked_id[CPARSE_CURRENT_UNIT];
                uint32_t const fn_id     = cparse->tracked_id[CPARSE_CURRENT_FN];

                switch (insert_ctbl(units, parse_chunk, unit_id, fn_id, CTBL_BEHAVIOR_MULTIPLE)) {
                    #ifndef NDEBUG
                        case CTBL_INSERT_ERROR:
                            VERBOSE_MSG_LITERAL("CPARSE_TBL_INSERT_ERROR");
                            TERMINATE_ERROR;
                    #endif
                    case CTBL_INSERT_DUPLICATE_ENTRY:
                        VERBOSE_MSG_VARIADIC(
                            "CPARSE_TBL_DUPLICATE_ENTRY: %s -> %s()",
                            get_chunk(parse_chunk, unit_id), get_chunk(parse_chunk, fn_id)
                        );
                        TERMINATE_ERROR;
                    case CTBL_INSERT_OK:
                        VERBOSE_MSG_VARIADIC(
                            "CPARSE_TBL_UNIQUE_ENTRY: %s -> %s()",
                            get_chunk(parse_chunk, unit_id), get_chunk(parse_chunk, fn_id)
                        );
                        break;
                    default:
                        VERBOSE_MSG_LITERAL("CPARSE_UNKNOWN_ERROR @ insert_ctbl(units, ...)");
                        TERMINATE_ERROR;
                }

                switch (insert_ctbl(fns, parse_chunk, fn_id, unit_id, CTBL_BEHAVIOR_MULTIPLE)) {
                    #ifndef NDEBUG
                        case CTBL_INSERT_ERROR:
                            VERBOSE_MSG_LITERAL("CPARSE_TBL_INSERT_ERROR");
                            TERMINATE_ERROR;
                    #endif
                    case CTBL_INSERT_DUPLICATE_ENTRY:
                        VERBOSE_MSG_VARIADIC(
                            "CPARSE_TBL_DUPLICATE_ENTRY: %s() -> %s",
                            get_chunk(parse_chunk, fn_id), get_chunk(parse_chunk, unit_id)
                        );
                        TERMINATE_ERROR;
                    case CTBL_INSERT_OK:
                        VERBOSE_MSG_VARIADIC(
                            "CPARSE_TBL_UNIQUE_ENTRY: %s() -> %s",
                            get_chunk(parse_chunk, fn_id), get_chunk(parse_chunk, unit_id)
                        );
                        break;
                    default:
                        VERBOSE_MSG_LITERAL("CPARSE_UNKNOWN_ERROR @ insert_ctbl(fns, ...)");
                        TERMINATE_ERROR;
                }
            }
    }

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_NAME)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_params_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_PARAM_LIST)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_return_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_RETURN)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_switch_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_SWITCH)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_type_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_TYPE)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}
static void end_while_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    DEBUG_ERROR_IF(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS) != C_WHILE)
    NDEBUG_EXECUTE(popStack_cparse(cparse, CPARSE_STACK_ELEMENTS))
}

void appendIfPossible_cparse(
    CParse* const cparse, char const* const str, uint64_t const len
) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    /* Reject empty statements */
    if (len == 1 && str[0] == ';') return;

    switch (peekStack_cparse(cparse, CPARSE_STACK_ELEMENTS)) {
        case C_CONDITION:
        case C_DECL_STMT:
        case C_EXPR_STMT:
        case C_INCR:
        case C_INIT:
        case C_NAME:
        case C_RETURN:
        case C_TYPE:
            DEBUG_ERROR_IF(
                append_chunk(cparse->chunks + CPARSE_CHUNK_PARSE, str, len) == NULL
            )
            NDEBUG_EXECUTE(
                append_chunk(cparse->chunks + CPARSE_CHUNK_PARSE, str, len)
            )
            break;
    }
}

uint32_t complexity_cparse(CParse const* const cparse, bool const interprocedural) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    uint32_t count = getEdgeCount_cparse(cparse, interprocedural);
    count += 2 * getFnCount_cparse(cparse);
    count -= getStmtCount_cparse(cparse);
    return count;
}

uint32_t complexityFn_cparse(
    CParse const* const cparse, uint32_t const fn_id, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(fn_id == 0xFFFFFFFF)

    uint32_t count = getEdgeCountOfFn_cparse(cparse, fn_id, interprocedural);
    count += 2;
    count -= getStmtCountOfFn_cparse(cparse, fn_id);
    return count;
}

uint32_t complexityUnit_cparse(
    CParse const* const cparse, uint32_t const unit_id, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(unit_id == 0xFFFFFFFF)

    uint32_t count = getEdgeCountOfUnit_cparse(cparse, unit_id, interprocedural);
    count += 2 * getFnCountOfUnit_cparse(cparse, unit_id);
    count -= getStmtCountOfUnit_cparse(cparse, unit_id);
    return count;
}

void constructEmpty_cparse(
    CParse*        cparse,
    uint32_t const chunk_size_guess,
    uint32_t const chunk_item_count_guess,
    uint32_t const unit_count_guess,
    uint32_t const total_fn_count_guess,
    uint32_t const load_percent,
    uint32_t const initial_stack_cap
) {
    DEBUG_ERROR_IF(chunk_size_guess == 0)
    DEBUG_ERROR_IF(chunk_size_guess == 0xFFFFFFFF)

    DEBUG_ERROR_IF(chunk_item_count_guess == 0)
    DEBUG_ERROR_IF(chunk_item_count_guess == 0xFFFFFFFF)

    DEBUG_ERROR_IF(unit_count_guess == 0)
    DEBUG_ERROR_IF(unit_count_guess == 0xFFFFFFFF)

    DEBUG_ERROR_IF(total_fn_count_guess == 0)
    DEBUG_ERROR_IF(total_fn_count_guess == 0xFFFFFFFF)

    DEBUG_ERROR_IF(initial_stack_cap == 0)
    DEBUG_ERROR_IF(initial_stack_cap == 0xFFFFFFFF)

    DEBUG_ERROR_IF(load_percent == 0)
    DEBUG_ERROR_IF(load_percent == 0xFFFFFFFF)

    DEBUG_ERROR_IF(cparse == NULL)

    cparse->interpretations_cap  = chunk_item_count_guess;
    cparse->interpretations_size = 0;
    cparse->interpretations      = malloc(chunk_item_count_guess);
    DEBUG_ERROR_IF(cparse->interpretations == NULL)

    for (unsigned stack_id = 0; stack_id <= CPARSE_STACK_LAST; stack_id++) {
        cparse->stack_cap[stack_id] = initial_stack_cap;
        cparse->stack_size[stack_id] = 0;
        cparse->stack[stack_id] = malloc(initial_stack_cap * sizeof(uint32_t));
        DEBUG_ERROR_IF(cparse->stack[stack_id] == NULL)
    }

    DEBUG_ASSERT_NDEBUG_EXECUTE(
        constructEmpty_chunk(
            cparse->chunks + CPARSE_CHUNK_PARSE, chunk_size_guess, chunk_item_count_guess
        )
    )
    DEBUG_ASSERT_NDEBUG_EXECUTE(
        constructEmpty_chunk(
            cparse->chunks + CPARSE_CHUNK_CALLS, CHUNK_RECOMMENDED_INITIAL_CAP, initial_stack_cap
        )
    )

    ChunkTable* const units = cparse->tables + CPARSE_TABLE_UNITS;
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_ctbl(units, unit_count_guess, load_percent))

    ChunkTable* const fns = cparse->tables + CPARSE_TABLE_FNS;
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_ctbl(fns, total_fn_count_guess, load_percent))

    ChunkTable* const fn_first_stmt = cparse->tables + CPARSE_TABLE_FN_FIRST_STMT;
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_ctbl(fn_first_stmt, total_fn_count_guess, load_percent))

    ChunkTable* const labels = cparse->tables + CPARSE_TABLE_LABELS;
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_ctbl(labels, initial_stack_cap, load_percent))

    Map* const edges = cparse->maps + CPARSE_MAP_EDGES;
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_map(edges, chunk_size_guess))

    for (unsigned i = 0; i <= CPARSE_TRACKED_LAST; i++)
        cparse->tracked_id[i] = 0xFFFFFFFF;
}

void end_cparse(CParse* const cparse, char const* const c_element_tag) {
    static CParser element_parsers[C_ELEMENT_COUNT] = C_ELEMENT_ENDERS;

    DEBUG_ASSERT(isValid_cparse(cparse))

    char const** bsearch_result = bsearch(
        c_element_tag, element_tags, C_ELEMENT_COUNT,
        sizeof(char const*), strcmp_as_comparator
    );
    if (bsearch_result == NULL) {
        VERBOSE_MSG_VARIADIC("CPARSE_IGNORE => <%s>", c_element_tag);
        return;
    }

    uint32_t const id = (uint32_t)(bsearch_result - element_tags);
    VERBOSE_MSG_VARIADIC("CPARSE_POP_ELEMENT => </%s>", element_tags[id]);
    element_parsers[id](cparse);
}

void endUnit_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    cparse->tracked_id[CPARSE_CURRENT_UNIT] = 0xFFFFFFFF;
}

void flush_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    cparse->interpretations_size = 0;
    for (int chunk_id = CPARSE_CHUNK_LAST; chunk_id >= 0; chunk_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(cparse->chunks + chunk_id))

    for (int tbl_id = CPARSE_TABLE_LAST; tbl_id >= 0; tbl_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_ctbl(cparse->tables + tbl_id))

    for (int map_id = CPARSE_MAP_LAST; map_id >= 0; map_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_map(cparse->maps + map_id))

    for (int stack_id = CPARSE_STACK_LAST; stack_id >= 0; stack_id--)
        cparse->stack_size[stack_id] = 0;

    for (int i = CPARSE_TRACKED_LAST; i >= 0; i--)
        cparse->tracked_id[i] = 0xFFFFFFFF;
}

void free_cparse(CParse* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    free(cparse->interpretations);

    for (int chunk_id = CPARSE_CHUNK_LAST; chunk_id >= 0; chunk_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(free_chunk(cparse->chunks + chunk_id))

    for (int tbl_id = CPARSE_TABLE_LAST; tbl_id >= 0; tbl_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(free_ctbl(cparse->tables + tbl_id))

    for (int map_id = CPARSE_MAP_LAST; map_id >= 0; map_id--)
        DEBUG_ASSERT_NDEBUG_EXECUTE(free_map(cparse->maps + map_id))

    for (int stack_id = CPARSE_STACK_LAST; stack_id >= 0; stack_id--)
        free(cparse->stack[stack_id]);
}

void generateDot_cparse(
    CParse const* const cparse, char const* filename, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
}

void generateXml_cparse(
    CParse const* const cparse, char const* filename, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))
}

uint32_t getCallCount_cparse(CParse const* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getCallCountOfFn_cparse(CParse const* const cparse, uint32_t const fn_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getCallCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getEdgeCount_cparse(CParse const* const cparse, bool const interprocedural) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getEdgeCountOfFn_cparse(
    CParse const* const cparse, uint32_t const fn_id, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getEdgeCountOfUnit_cparse(
    CParse const* const cparse, uint32_t const unit_id, bool const interprocedural
) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getFnCount_cparse(CParse const* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getFnCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getStmtCount_cparse(CParse const* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getStmtCountOfFn_cparse(CParse const* const cparse, uint32_t const fn_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getStmtCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

uint32_t getUnitCount_cparse(CParse const* const cparse) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    return 0;
}

bool isValid_cparse(CParse const* const cparse) {
    if (cparse == NULL)                                             return 0;
    if (cparse->interpretations == NULL)                            return 0;
    if (cparse->interpretations_cap == 0)                           return 0;
    if (cparse->interpretations_cap == 0xFFFFFFFF)                  return 0;
    if (cparse->interpretations_size > cparse->interpretations_cap) return 0;

    for (int chunk_id = CPARSE_CHUNK_LAST; chunk_id >= 0; chunk_id--)
        if (!isValid_chunk(cparse->chunks + chunk_id)) return 0;

    for (int tbl_id = CPARSE_TABLE_LAST; tbl_id >= 0; tbl_id--)
        if (!isValid_ctbl(cparse->tables + tbl_id)) return 0;

    for (int map_id = CPARSE_MAP_LAST; map_id >= 0; map_id--)
        if (!isValid_map(cparse->maps + map_id)) return 0;

    for (int stack_id = CPARSE_STACK_LAST; stack_id >= 0; stack_id--) {
        if (cparse->stack[stack_id] == NULL)                            return 0;
        if (cparse->stack_cap[stack_id] == 0)                           return 0;
        if (cparse->stack_cap[stack_id] == 0xFFFFFFFF)                  return 0;
        if (cparse->stack_size[stack_id] > cparse->stack_cap[stack_id]) return 0;
    }

    return 1;
}

void start_cparse(CParse* const cparse, char const* const c_element_tag) {
    DEBUG_ASSERT(isValid_cparse(cparse))
    DEBUG_ERROR_IF(c_element_tag == NULL)

    static CParser element_parsers[C_ELEMENT_COUNT] = C_ELEMENT_STARTERS;

    char const** bsearch_result = bsearch(
        c_element_tag, element_tags, C_ELEMENT_COUNT,
        sizeof(char const*), strcmp_as_comparator
    );
    if (bsearch_result == NULL) {
        VERBOSE_MSG_VARIADIC("CPARSE_IGNORE => <%s>", c_element_tag);
        return;
    }

    uint32_t const id = (uint32_t)(bsearch_result - element_tags);
    VERBOSE_MSG_VARIADIC("CPARSE_PUSH_ELEMENT => <%s>", element_tags[id]);
    element_parsers[id](cparse);
}

void startUnit_cparse(
    CParse* const cparse, char const* const unit_name, uint64_t const unit_len
) {
    DEBUG_ASSERT(isValid_cparse(cparse))

    if (unit_name == NULL || unit_len == 0) {
        VERBOSE_MSG_LITERAL("CPARSE_NULL_OR_EMPTY_UNIT_NAME");
        TERMINATE_ERROR;
    } else if (cparse->tracked_id[CPARSE_CURRENT_UNIT] != 0xFFFFFFFF) {
        VERBOSE_MSG_LITERAL("CPARSE_NESTED_UNIT (How did you do that!?)");
        TERMINATE_ERROR;
    }

    VERBOSE_MSG_VARIADIC("CPARSE_START_UNIT %.*s", (int)unit_len, unit_name);

    parse_cparse(cparse, unit_name, unit_len, C_UNIT);

    cparse->tracked_id[CPARSE_CURRENT_UNIT] = CPARSE_LAST_CHUNK_ID;
    DEBUG_ERROR_IF(cparse->tracked_id[CPARSE_CURRENT_UNIT] == 0xFFFFFFFF)
}
