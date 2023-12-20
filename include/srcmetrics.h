/**
 * @file srcmetrics.h
 * @brief Defines Options and the initial options before command-line arguments.
 * @author Yavuz Koroglu
 */
#ifndef SRCMETRICS_H
    #define SRCMETRICS_H
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include "libsrcml/srcml.h"
    #include "padkit/bliterals.h"
    #include "padkit/chunk.h"
    #include "padkit/timestamp.h"

    #define ALL_METRICS_ENABLED     B8(B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111)

    #define FLAG_GRAPH_ENABLE_DOT   B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000001)
    #define FLAG_GRAPH_ENABLE_XML   B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000010)
    #define FLAG_CG_ENABLE          B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000100)
    #define FLAG_CG_NO_EXTERNAL     B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00001000)
    #define FLAG_CFG_ENABLE         B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00010000)
    #define FLAG_IPCFG_ENABLE       B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00100000)
    #define FLAG_RFU_SIMPLE         B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_01000000)
    #define FLAG_RFU_SHOW           B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_10000000)
    #define FLAG_CC_SHOW            B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000001,B_00000000)
    #define FLAG_VERBOSE            B8(B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000000,B_00000010,B_00000000)

    #define FLAG_GRAPH_DISABLE_DOT  ~FLAG_GRAPH_ENABLE_DOT
    #define FLAG_GRAPH_DISABLE_XML  ~FLAG_GRAPH_ENABLE_XML
    #define FLAG_CG_DISABLE         ~FLAG_CG_ENABLE
    #define FLAG_CFG_DISABLE        ~(FLAG_CFG_ENABLE | FLAG_IPCFG_ENABLE)
    #define FLAG_RFU_TRANSITIVE     ~FLAG_RFU_SIMPLE
    #define FLAG_RFU_QUIET          ~FLAG_RFU_SHOW
    #define FLAG_CC_QUIET           ~FLAG_CC_SHOW

    #define FLAGS_DEFAULT           (FLAG_GRAPH_ENABLE_DOT | FLAG_GRAPH_ENABLE_XML | FLAG_CG_NO_EXTERNAL | FLAG_RFU_SIMPLE | FLAG_RFU_SHOW | FLAG_CC_SHOW)

    #define VERBOSE_MSG_LITERAL(string_literal)                     \
        if (isVerbose())                                            \
            fprintf(                                                \
                stderr, "[%s] " string_literal " @ %s::%s():%d\n",  \
                get_timestamp(), __FILE__, __func__, __LINE__       \
            )

    #define VERBOSE_MSG_VARIADIC(format_literal, ...)               \
        if (isVerbose())                                            \
            fprintf(                                                \
                stderr, "[%s] " format_literal " @ %s::%s():%d\n",  \
                get_timestamp(), __VA_ARGS__, __FILE__, __func__,   \
                __LINE__                                            \
            )

    /**
     * @def OPTIONS_INITIAL
     *   Initial options are no infiles with BUFSIZ capacity, no outfile, no language, standard out, and all metrics enabled.
     */
    #define OPTIONS_INITIAL         \
        ((struct Options){          \
            0xFFFFFFFF,             \
            0,                      \
            0,                      \
            BUFSIZ,                 \
            NULL,                   \
            NULL,                   \
            SRCML_LANGUAGE_NONE,    \
            ALL_METRICS_ENABLED,    \
            NULL,                   \
            NULL,                   \
            NULL,                   \
            FLAGS_DEFAULT           \
        })

    /**
     * @struct Options
     * @brief Global srcMetrics Options.
     *
     * Command-line parameters usually interact with these options.
     *
     * @see srcmetrics.c
     */
    extern struct Options {
        uint32_t      first_infile_id;
        uint32_t      last_infile_id;
        size_t        n_cmd_infiles;
        size_t        cap_cmd_infiles;
        char const**  cmd_infiles;
        char const*   outfile;
        char const*   language;
        uint_fast64_t enabledMetrics;
        char const*   cg_name;
        char const*   cfg_name;
        char const*   ipcfg_name;
        uint_fast64_t flags;
    } options;

    extern Chunk strings[1];

    /**
     * @brief Checks if control flow graphs are enabled.
     */
    bool isCFGEnabled(void);

    /**
     * @brief Checks if call graphs are enabled.
     */
    bool isCGEnabled(void);

    /**
     * @brief Checks if cg-no-external is toggled.
     */
    bool isCGNoExternal(void);

    /**
     * @brief Checks if DOT graphs are enabled.
     */
    bool isDotEnabled(void);

    /**
     * @brief Checks if inter-procedural control flow graphs are enabled.
     */
    bool isIPCFGEnabled(void);

    /**
     * @brief Checks if RFU-quiet is toggled.
     */
    bool isRFUQuiet(void);

    /**
     * @brief Checks if RFU-simple is toggled.
     */
    bool isRFUSimple(void);

    /**
     * @brief Checks if verbose status outputs are enabled.
     */
    bool isVerbose(void);

    /**
     * @brief Checks if XML graphs are enabled.
     */
    bool isXmlEnabled(void);
#endif
