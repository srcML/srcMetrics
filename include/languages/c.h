#ifndef C_H
    #define C_H
    #include "padkit/map.h"
    #include "padkit/chunkset.h"
    #include "padkit/chunktable.h"

    #define CPARSE_RECOMMENDED_CHUNK_SIZE        16777216

    #define CPARSE_RECOMMENDED_CHUNK_ITEM_COUNT  2097152

    #define CPARSE_RECOMMENDED_INITIAL_STACK_CAP 16

    #define CPARSE_RECOMMENDED_LOAD_PERCENT      75

    /* C Language Elements */
    #define C_BLOCK_CONTENT 0
    #define C_BREAK         1
    #define C_CALL          2
    #define C_CASE          3
    #define C_CONDITION     4
    #define C_CONTINUE      5
    #define C_CONTROL       6
    #define C_DECL_STMT     7
    #define C_DEFAULT       8
    #define C_DO            9
    #define C_ELSE          10
    #define C_ELSEIF        11
    #define C_EMPTY_STMT    12
    #define C_EXPR_STMT     13
    #define C_FOR           14
    #define C_FUNCTION      15
    #define C_GOTO          16
    #define C_IF            17
    #define C_IF_STMT       18
    #define C_INCR          19
    #define C_INIT          20
    #define C_LABEL         21
    #define C_NAME          22
    #define C_PARAM_LIST    23
    #define C_RETURN        24
    #define C_SWITCH        25
    #define C_TYPE          26
    #define C_WHILE         27
    #define C_LAST_ELEMENT  C_WHILE

    #define C_ELEMENT_COUNT C_LAST_ELEMENT + 1

    #define C_IS_VALID_ELEMENT(element) (element <= C_LAST_ELEMENT)

    /* Chunk Interpretations */
    #define C_CALL_NAME             0
    #define C_COND                  1
    #define C_FN_NAME               2
    #define C_FN_PARAMS             3
    #define C_FN_TYPE               4
    #define C_SPURIOUS              5
    #define C_STMT                  6
    #define C_UNIT                  7
    #define C_LAST_INTERPRETATION   C_UNIT

    #define C_INTERPRETATION_COUNT  C_LAST_INTERPRETATION + 1

    #define C_IS_VALID_INTERPRETATION(interpretation) (interpretation <= C_LAST_INTERPRETATION)

    #define C_ELEMENT_TAGS {                                                                                \
        "block_content", "break", "call", "case", "condition", "continue", "control", "decl_stmt",          \
        "default", "do", "else", "elseif", "empty_stmt", "expr_stmt", "for", "function", "goto", "if",      \
        "if_stmt", "incr", "init", "label", "name", "parameter_list", "return", "switch", "type", "while"   \
    }

    #define C_INTERPRETATION_NAMES {                                                                        \
        "C_CALL_NAME", "C_COND", "C_FN_NAME", "C_FN_PARAMS", "C_FN_TYPE", "C_SPURIOUS", "C_STMT", "C_UNIT"  \
    }

    #define NOT_A_CPARSE ((CParse){                                                      \
                            0, 0, NULL,                                                  \
                            { NOT_A_CHUNK, NOT_A_CHUNK },                                \
                            { NOT_A_CHUNK_TABLE, NOT_A_CHUNK_TABLE, NOT_A_CHUNK_TABLE }, \
                            { NOT_A_MAP },                                               \
                            { 0, 0, 0 }, { 0, 0, 0 }, { NULL, NULL, NULL }, { 0, 0, 0 }, \
                         })

    #define CPARSE_CHUNK_PARSE              0
    #define CPARSE_CHUNK_CALLS              1
    #define CPARSE_CHUNK_LAST               CPARSE_CHUNK_CALLS
    #define CPARSE_TABLE_UNITS              0
    #define CPARSE_TABLE_FNS                1
    #define CPARSE_TABLE_FN_FIRST_STMT      2
    #define CPARSE_TABLE_FN_LAST_STMT       3
    #define CPARSE_TABLE_LABELS             4
    #define CPARSE_TABLE_LAST               CPARSE_TABLE_LABELS
    #define CPARSE_MAP_EDGES                0
    #define CPARSE_MAP_LAST                 CPARSE_MAP_EDGES
    #define CPARSE_STACK_ELEMENTS           0
    #define CPARSE_STACK_INCOMING           1
    #define CPARSE_STACK_LOOPS              2
    #define CPARSE_STACK_LAST               CPARSE_STACK_LOOPS
    #define CPARSE_CURRENT_UNIT             0
    #define CPARSE_CURRENT_FN               1
    #define CPARSE_CURRENT_FN_FIRST_STMT    2
    #define CPARSE_TRACKED_LAST             CPARSE_CURRENT_FN_FIRST_STMT
    typedef struct CParseBody {
        uint32_t   interpretations_cap;
        uint32_t   interpretations_size;
        uint8_t*   interpretations;
        Chunk      chunks     [CPARSE_CHUNK_LAST + 1];
        ChunkTable tables     [CPARSE_TABLE_LAST + 1];
        Map        maps       [CPARSE_MAP_LAST + 1];
        uint32_t   stack_cap  [CPARSE_STACK_LAST + 1];
        uint32_t   stack_size [CPARSE_STACK_LAST + 1];
        uint32_t*  stack      [CPARSE_STACK_LAST + 1];
        uint32_t   tracked_id [CPARSE_TRACKED_LAST + 1];
    } CParse;

    #define CPARSE_LAST_CHUNK_ID cparse->chunks[CPARSE_CHUNK_PARSE].nStrings - 1

    typedef void(*CParser)(CParse* const);

    #define C_ELEMENT_STARTERS {        \
        &start_block_content_cparse,    \
        &start_break_cparse,            \
        &start_call_cparse,             \
        &start_case_cparse,             \
        &start_condition_cparse,        \
        &start_continue_cparse,         \
        &start_for_ctrl_cparse,         \
        &start_decl_stmt_cparse,        \
        &start_default_cparse,          \
        &start_do_cparse,               \
        &start_else_cparse,             \
        &start_elseif_cparse,           \
        &start_empty_stmt_cparse,       \
        &start_expr_stmt_cparse,        \
        &start_for_cparse,              \
        &start_fn_cparse,               \
        &start_goto_cparse,             \
        &start_if_cparse,               \
        &start_if_stmt_cparse,          \
        &start_incr_cparse,             \
        &start_init_cparse,             \
        &start_label_cparse,            \
        &start_name_cparse,             \
        &start_params_cparse,           \
        &start_return_cparse,           \
        &start_switch_cparse,           \
        &start_type_cparse,             \
        &start_while_cparse             \
    }

    #define C_ELEMENT_ENDERS {          \
        &end_block_content_cparse,      \
        &end_break_cparse,              \
        &end_call_cparse,               \
        &end_case_cparse,               \
        &end_condition_cparse,          \
        &end_continue_cparse,           \
        &end_for_ctrl_cparse,           \
        &end_decl_stmt_cparse,          \
        &end_default_cparse,            \
        &end_do_cparse,                 \
        &end_else_cparse,               \
        &end_elseif_cparse,             \
        &end_empty_stmt_cparse,         \
        &end_expr_stmt_cparse,          \
        &end_for_cparse,                \
        &end_fn_cparse,                 \
        &end_goto_cparse,               \
        &end_if_cparse,                 \
        &end_if_stmt_cparse,            \
        &end_incr_cparse,               \
        &end_init_cparse,               \
        &end_label_cparse,              \
        &end_name_cparse,               \
        &end_params_cparse,             \
        &end_return_cparse,             \
        &end_switch_cparse,             \
        &end_type_cparse,               \
        &end_while_cparse               \
    }

    void appendIfPossible_cparse(CParse* const cparse, char const* const str, uint64_t const len);

    uint32_t complexity_cparse(CParse const* const cparse, bool const interprocedural);

    uint32_t complexityFn_cparse(CParse const* const cparse, uint32_t const fn_id, bool const interprocedural);

    uint32_t complexityUnit_cparse(CParse const* const cparse, uint32_t const unit_id, bool const interprocedural);

    void constructEmpty_cparse(
        CParse*        cparse,
        uint32_t const chunk_size_guess,
        uint32_t const chunk_item_count_guess,
        uint32_t const unit_count_guess,
        uint32_t const total_fn_count_guess,
        uint32_t const load_percent,
        uint32_t const initial_stack_cap
    );

    void end_cparse(CParse* const cparse, char const* const c_element_tag);

    void endUnit_cparse(CParse* const cparse);

    void flush_cparse(CParse* const cparse);

    void free_cparse(CParse* const cparse);

    void generateDot_cparse(CParse const* const cparse, char const* filename, bool const interprocedural);

    void generateXml_cparse(CParse const* const cparse, char const* filename, bool const interprocedural);

    uint32_t getCallCount_cparse(CParse const* const cparse);

    uint32_t getCallCountOfFn_cparse(CParse const* const cparse, uint32_t const fn_id);

    uint32_t getCallCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id);

    uint32_t getEdgeCount_cparse(CParse const* const cparse, bool const interprocedural);

    uint32_t getEdgeCountOfFn_cparse(CParse const* const cparse, uint32_t const fn_id, bool const interprocedural);

    uint32_t getEdgeCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id, bool const interprocedural);

    uint32_t getFnCount_cparse(CParse const* const cparse);

    uint32_t getFnCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id);

    uint32_t getStmtCount_cparse(CParse const* const cparse);

    uint32_t getStmtCountOfFn_cparse(CParse const* const cparse, uint32_t const fn_id);

    uint32_t getStmtCountOfUnit_cparse(CParse const* const cparse, uint32_t const unit_id);

    uint32_t getUnitCount_cparse(CParse const* const cparse);

    bool isValid_cparse(CParse const* const cparse);

    void start_cparse(CParse* const cparse, char const* const c_element_tag);

    void startUnit_cparse(CParse* const cparse, char const* const unit_name, uint64_t const unit_len);
#endif
