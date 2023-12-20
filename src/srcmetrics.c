/**
 * @file srcmetrics.c
 * @brief Implements the main() function.
 *
 * @copyright Copyright (C) 2023 srcML, LLC. (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @mainpage srcMetrics
 *
 * The functions in srcmetrics are for the purposes of:
 *
 * * Obtaining static metrics from the source code and its control-flow/call graphs.
 */
#include <ctype.h>

#include "padkit/chunk.h"
#include "padkit/csv.h"
#include "padkit/reallocate.h"
#include "padkit/streq.h"
#include "srcmetrics.h"
#include "srcmetrics/event.h"
#include "srcmetrics/metrics.h"
#include "srcmetrics/report.h"

char const* csv_delimeter = CSV_INITIAL_DELIMETER;
char const* csv_row_end   = CSV_INITIAL_ROW_END;
struct Options options    = OPTIONS_INITIAL;
Chunk strings[1]          = { NOT_A_CHUNK };

/**
 * @defgroup atexit_Functions Functions Called @ Exit
 * @{
 */

/**
 * @brief Frees the global strings Chunk.
 */
static void free_strings(void) {
    DEBUG_ABORT_IF(!free_chunk(strings))
    NDEBUG_EXECUTE(free_chunk(strings))
}

/**
 * @brief Frees the infiles array from the options.
 */
static void free_infiles(void) {
    DEBUG_ABORT_IF(!options.cmd_infiles)
    free(options.cmd_infiles);
    options.cmd_infiles = NULL;
}

/**@}*/

/**
 * @defgroup Message_Functions Message Functions
 * @{
 */

/**
 * @brief Prints a bare short option error.
 */
static void showBareShortOptionError(void) {
    fputs("\n"
          "Unrecognized bare option '-'.\n"
          "\n"
          "Execute `srcmetrics --help` for more information.\n"
          "\n", stderr);
}

/**
 * @brief Prints the copyright message.
 */
static void showCopyright(void) {
    fputs("\n"
          "Copyright (C) 2023 srcML, LLC. (www.srcML.org)\n"
          "\n", stderr);
}

/**
 * @brief Prints an abbreviation-description pair.
 */
static void showDescription(char const* const abbreviation, char const* const description) {
    fprintf(stderr, "%*s: %s\n", 5, abbreviation, description);
}

/**
 * @brief Prints a 'file-NOT-found' error.
 */
static void showFileNOTFoundError(char const* const filepath) {
    fprintf(stderr, "\n"
                    "File '%s' is NOT found\n"
                    "\n", filepath);
}

/**
 * @brief Prints a short option 'must-be-alone' error.
 */
static void showLongOptionMustBeAloneError(char const* const option_str) {
    fprintf(stderr, "\n"
                    "Long option '%s' must NOT be used with any other option.\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", option_str);
}

/**
 * @brief Prints a lomg option 'needs-parameters' error.
 */
static void showLongOptionNeedsParametersError(char const* const option_str) {
    fprintf(stderr, "\n"
                    "Short option '%s' must be followed with parameter(s).\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", option_str);
}

/**
 * @brief Prints the long help message.
 */
static void showLongHelpMessage(void) {
    fputs("\n"
          "Usage: srcmetrics [options] <src_infile>...\n"
          "\n"
          "Calculates static metrics from C source code files.\n"
          "\n"
          "Source-code input can be from standard input, a file, a directory, or an archive file, i.e., tar, cpio, zip, etc.\n"
          "\n"
          "GENERAL OPTIONS:\n"
          "  -h,--help                      Output this help message and exit\n"
          "  -V,--version                   Output version number and exit\n"
          "  -v,--verbose                   Status information to stderr\n"
          "  -c,--copyright                 Output the copyright message and exit\n"
          "  -o,--output FILE               Write output to FILE\n"
          "  -l,--language LANG             Set the source-code language to C\n"
          "  -d,--delimeter DELIM           Change the CSV delimeter, default: ','\n"
          "  --files-from FILE              Input source-code filenames from FILE\n"
          "\n"
          "SRCMETRICS OPTIONS:\n"
          "  -a,--all-metrics               (Default) Report all metrics (implies '--RFU-show --CC-show')\n"
          "  -m,--metric METRIC             Report this METRIC if possible, excludes all unspecified metrics\n"
          "  -e,--exclude METRIC            Exclude this METRIC from the output\n"
          "  -f,--metrics-from FILE         Input enabled metrics from FILE\n"
          "  -x,--excluded-from FILE        Input excluded metrics from FILE\n"
          "\n"
          "METADATA OPTIONS:\n"
          "  -L,--list                      Output the list of supported metrics and exit\n"
          "  -s,--show METRIC               Output detailed information on METRIC and exit\n"
          "\n"
          "GRAPH OPTIONS:\n"
          "     --graph-enable-dot          (Default) Enables .dot output\n"
          "     --graph-disable-dot         Disables .dot output\n"
          "     --graph-enable-xml          (Default) Enables .xml output\n"
          "     --graph-disable-xml         Disables .xml output\n"
          "\n"
          "CALL GRAPH OPTIONS:\n"
          "     --no-cg                     (Default) Does NOT compute the call graph\n"
          "     --cg <graph_name>           Computes one overall call graph (implies '-m RFU --RFU-quiet')\n"
          "     --cg-no-external            (Default) Ignores external calls in the call graph\n"
          "     --cg-show-external          Shows external calls in the call graph\n"
          "\n"
          "CONTROL FLOW GRAPH OPTIONS: \n"
          "     --no-cfg                    (Default) Does NOT output control flow graphs\n"
          "     --cfg <graph_name>          Outputs the control flow graph (implies '-m CC --CC-quiet')\n"
          "     --ipcfg <graph_name>        Outputs the inter-procedural control flow graph (implies '-m CC --CC-quiet')\n"
          "\n"
          "RFU OPTIONS:\n"
          "     --RFU-show                  (Default) Show RFU metrics\n"
          "     --RFU-quiet                 Do NOT output any RFU metrics (for CG generation)\n"
          "     --RFU-simple                (Default) Non-CG based RFU estimation\n"
          "     --RFU-transitive            CG based RFU estimation\n"
          "\n"
          "CC OPTIONS:\n"
          "     --CC-show                   (Default) Show CC metrics\n"
          "     --CC-quiet                  Do NOT output any CC metrics (for CFG generation)\n"
          "\n"
          "Have a question or need to report a bug?\n"
          "Contact us at www.srcml.org/support.html\n"
          "www.srcML.org\n"
          "\n", stderr);
}

/**
 * @brief Prints a 'Metric-NOT-found' error.
 */
static void showMetricNOTFoundError(char const* const abbreviation) {
    fprintf(stderr, "\n"
                    "Metric '%s' is NOT supported\n"
                    "\n", abbreviation);
}

/**
 * @brief Prints the short help message.
 */
static void showShortHelpMessage(void) {
    fputs("\n"
          "'srcmetrics' typically accepts input from pipe, not a terminal.\n"
          "Typical usage includes:\n"
          "\n"
          "    # Compute all non-graph metrics of a source file and print it to standard out.\n"
          "    srcmetrics examples/prime_preprocessed.c\n"
          "\n"
          "    # Compute the call graph of several source files.\n"
          "    srcmetrics --cg examples/cg examples/" "*.c\n"
          "\n"
          "    # Compute the control flow graph of several source files.\n"
          "    srcmetrics --cfg examples/ examples/" "*.c\n"
          "\n"
          "    # Compute everything!\n"
          "    srcmetrics --cg examples/cg --cfg examples/cfg --ipcfg examples/ipcfg --all-metrics examples/" "*.c\n"
          "\n"
          "Execute `srcmetrics --help` for more information.\n"
          "\n", stderr);
}

/**
 * @brief Prints a short option 'must-be-alone' error.
 */
static void showShortOptionMustBeAloneError(char const short_option) {
    fprintf(stderr, "\n"
                    "Short option '-%c' must NOT be used with any other option.\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", short_option);
}

/**
 * @brief Prints a short option 'needs-parameters' error.
 */
static void showShortOptionNeedsParametersError(char const short_option) {
    fprintf(stderr, "\n"
                    "Short option '-%c' must be followed with parameter(s).\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", short_option);
}

/**
 * @brief Prints an unrecognized long option error.
 */
static void showUnrecognizedLongOptionError(char const* const option_str) {
    fprintf(stderr, "\n"
                    "Unrecognized long option '%s'\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", option_str);
}

/**
 * @brief Prints an unrecognized short option error.
 */
static void showUnrecognizedShortOptionError(char const short_option) {
    fprintf(stderr, "\n"
                    "Unrecognized short option '-%c'\n"
                    "\n"
                    "Execute `srcmetrics --help` for more information.\n"
                    "\n", short_option);
}

/**
 * @brief Prints the program and library versions.
 */
static void showVersion(void) {
    fprintf(stderr, "\n"
                    "srcmetrics "VERSION_SRCMETRICS"\n"
                    "libsrcml %s\n"
                    "\n", srcml_version_string());
}

/**@}*/

/**
 * @brief Gets infiles using the file given with '--files-from' argument.
 *
 * WARNING: Does NOT check if valid file name (e.g. '>' and '&' characters)!!
 *
 * @param filename The '--files-from' file name.
 */
static void getInfilesFromFile(char const* const filename) {
    FILE* const stream = fopen(filename, "r");
    if (stream == NULL) { showFileNOTFoundError(filename); exit(EXIT_FAILURE); }

    options.first_infile_id = strings->nStrings;

    DEBUG_ERROR_IF(fromStream_chunk(strings, stream, NULL) == 0xFFFFFFFF)
    NDEBUG_EXECUTE(fromStream_chunk(strings, stream, NULL))

    DEBUG_ERROR_IF(options.first_infile_id == strings->nStrings)

    DEBUG_ERROR_IF(fclose(stream) == EOF)
    NDEBUG_EXECUTE(fclose(stream))

    options.last_infile_id = strings->nStrings - 1;
}

/**
 * @brief Gets enabled or excluded metrics using the file given with '--metrics-from' argument.
 *
 * WARNING: Does NOT check if valid file name (e.g. '>' and '&' characters)!!
 *
 * @param filename The '--metrics-from' file name.
 * @param enable Enable metrics if 1, exclude otherwise.
 */
static void getEnabledOrExcludedMetricsFromFile(char const* const filename, bool const enable) {
    FILE* const stream = fopen(filename, "r");
    if (stream == NULL) { showFileNOTFoundError(filename); exit(EXIT_FAILURE); }

    DEBUG_ERROR_IF(fromStream_chunk(strings, stream, NULL) == 0xFFFFFFFF)
    NDEBUG_EXECUTE(fromStream_chunk(strings, stream, NULL))

    DEBUG_ERROR_IF(fclose(stream) == EOF)
    NDEBUG_EXECUTE(fclose(stream))

    uint32_t const firstMetricId = strings->nStrings;
    for (uint32_t metricId = firstMetricId; metricId < strings->nStrings; metricId++) {
        char const* metric = get_chunk(strings, metricId);
        if (!enableOrExclude_metric(metric, enable)) {
            showMetricNOTFoundError(metric);
            exit(EXIT_FAILURE);
        }
    }
}

bool isCFGEnabled(void)   { return options.flags & FLAG_CFG_ENABLE; }
bool isCGEnabled(void)    { return options.flags & FLAG_CG_ENABLE; }
bool isCGNoExternal(void) { return options.flags & FLAG_CG_NO_EXTERNAL; }
bool isDotEnabled(void)   { return options.flags & FLAG_GRAPH_ENABLE_DOT; }
bool isIPCFGEnabled(void) { return options.flags & FLAG_IPCFG_ENABLE; }
bool isRFUQuiet(void)     { return !(options.flags & FLAG_RFU_SHOW); }
bool isRFUSimple(void)    { return options.flags & FLAG_RFU_SIMPLE; }
bool isVerbose(void)      { return options.flags & FLAG_VERBOSE; }
bool isXmlEnabled(void)   { return options.flags & FLAG_GRAPH_ENABLE_XML; }

/**
 * @brief Parses the command-line arguments and starts the metrics collection.
 * @param argc #arguments including the program name.
 * @param argv The list of arguments as an array of strings.
 * @return EXIT_FAILURE if something goes wrong, EXIT_SUCCESS otherwise.
 */
int main(int argc, char* argv[]) {
    /* There has to be some arguments, or print the short help message. */
    if (argc < 2) { showShortHelpMessage(); return EXIT_SUCCESS; }

    /* The id of the final argument. */
    int const finalArg_id = argc - 1;

    /* Construct a Chunk for all strings. */
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_chunk(strings, CHUNK_RECOMMENDED_PARAMETERS))

    /* Register the strings to be freed at the end. */
    DEBUG_ERROR_IF(atexit(free_strings) != 0)
    NDEBUG_EXECUTE(atexit(free_strings))

    /* Evaluate arguments */
    for (int arg_id = 1; arg_id <= finalArg_id; arg_id++) {
        switch (argv[arg_id][0]) {
            case '-':
                if (argv[arg_id][1] == '\0') {
                    showBareShortOptionError();
                    return EXIT_SUCCESS;
                }
                int i = 0;
SRCMETRICS_NEXT_OPTION_CHAR:
                i++;
                switch (argv[arg_id][i]) {
                    case '\0':
                        break;
                    case '-':
                        if (STR_EQ_CONST(argv[arg_id], "--all-metrics")) {
                            options.enabledMetrics |= ALL_METRICS_ENABLED;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--cfg")) {
                            if (arg_id < finalArg_id) {
                                options.flags |= FLAG_CFG_ENABLE;
                                DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("CC", 1))
                                options.flags &= FLAG_CC_QUIET;
                                arg_id++;
                                options.cfg_name = argv[arg_id];
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--cfg=")) {
                            options.flags |= FLAG_CFG_ENABLE;
                            DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("CC", 1))
                            options.flags &= FLAG_CC_QUIET;
                            options.cfg_name = argv[arg_id] + 6;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--cg")) {
                            if (arg_id < finalArg_id) {
                                options.flags |= FLAG_CG_ENABLE;
                                DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("RFU", 1))
                                options.flags &= FLAG_RFU_QUIET;
                                arg_id++;
                                options.cg_name = argv[arg_id];
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--cg=")) {
                            options.flags |= FLAG_CG_ENABLE;
                            DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("RFU", 1))
                            options.flags &= FLAG_RFU_QUIET;
                            options.cg_name = argv[arg_id] + 5;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--cg-no-external")) {
                            options.flags |= FLAG_CG_NO_EXTERNAL;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--cg-show-external")) {
                            options.flags &= (~FLAG_CG_NO_EXTERNAL);
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--copyright")) {
                            if (arg_id == 1 && arg_id == finalArg_id) {
                                showCopyright();
                                return EXIT_SUCCESS;
                            } else {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--CC-quiet")) {
                            options.flags &= FLAG_CC_QUIET;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--CC-show")) {
                            options.flags |= FLAG_CC_SHOW;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--delimeter")) {
                            if (arg_id < finalArg_id) {
                                csv_delimeter = argv[++arg_id];
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--delimeter=")) {
                            csv_delimeter = argv[arg_id] + 12;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--exclude")) {
                            if (arg_id < finalArg_id) {
                                arg_id++;
                                if (enableOrExclude_metric(argv[arg_id], 0)) {
                                    break;
                                } else {
                                    showMetricNOTFoundError(argv[arg_id]);
                                    return EXIT_FAILURE;
                                }
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--exclude=")) {
                            if (enableOrExclude_metric(argv[arg_id] + 10, 0)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id] + 10);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--excluded-from")) {
                            if (arg_id < finalArg_id) {
                                getEnabledOrExcludedMetricsFromFile(argv[++arg_id], 0);
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--excluded-from=")) {
                            getEnabledOrExcludedMetricsFromFile(argv[arg_id] + 16, 0);
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--files-from")) {
                            if (arg_id < finalArg_id) {
                                getInfilesFromFile(argv[++arg_id]);
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--files-from=")) {
                            getInfilesFromFile(argv[arg_id] + 13);
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--graph-enable-dot")) {
                            options.flags |= FLAG_GRAPH_ENABLE_DOT;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--graph-disable-dot")) {
                            options.flags &= FLAG_GRAPH_DISABLE_DOT;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--graph-enable-xml")) {
                            options.flags |= FLAG_GRAPH_ENABLE_XML;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--graph-disable-xml")) {
                            options.flags &= FLAG_GRAPH_DISABLE_XML;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--help")) {
                            if (arg_id == 1 && arg_id == finalArg_id) {
                                showLongHelpMessage();
                                return EXIT_SUCCESS;
                            } else {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--ipcfg")) {
                            if (arg_id < finalArg_id) {
                                options.flags |= FLAG_IPCFG_ENABLE;
                                DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("CC", 1))
                                options.flags &= FLAG_CC_QUIET;
                                arg_id++;
                                options.ipcfg_name = argv[arg_id];
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--ipcfg=")) {
                            options.flags |= FLAG_IPCFG_ENABLE;
                            DEBUG_ASSERT_NDEBUG_EXECUTE(enableOrExclude_metric("CC", 1))
                            options.flags &= FLAG_CC_QUIET;
                            options.ipcfg_name = argv[arg_id] + 6;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--language")) {
                            if (arg_id < finalArg_id) {
                                options.language = "C";
                                arg_id++;
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--language=")) {
                            options.language = "C";
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--list")) {
                            if (arg_id == 1 && arg_id == finalArg_id) {
                                showListOf_metrics();
                                return EXIT_SUCCESS;
                            } else {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--metric")) {
                            if (arg_id < finalArg_id) {
                                arg_id++;
                                if (enableOrExclude_metric(argv[arg_id], 1)) {
                                    break;
                                } else {
                                    showMetricNOTFoundError(argv[arg_id]);
                                    return EXIT_FAILURE;
                                }
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--metric=")) {
                            if (enableOrExclude_metric(argv[arg_id] + 10, 1)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id] + 10);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--metrics-from")) {
                            if (arg_id < finalArg_id) {
                                getEnabledOrExcludedMetricsFromFile(argv[++arg_id], 1);
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--metrics-from=")) {
                            getEnabledOrExcludedMetricsFromFile(argv[arg_id] + 15, 1);
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--no-cfg")) {
                            options.flags &= FLAG_CFG_DISABLE;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--no-cg")) {
                            options.flags &= FLAG_CG_DISABLE;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--output")) {
                            if (arg_id < finalArg_id) {
                                options.outfile = argv[++arg_id];
                                break;
                            } else {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--output=")) {
                            options.outfile = argv[arg_id] + 9;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--RFU-quiet")) {
                            options.flags &= FLAG_RFU_QUIET;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--RFU-show")) {
                            options.flags |= FLAG_RFU_SHOW;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--RFU-simple")) {
                            options.flags |= FLAG_RFU_SIMPLE;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--RFU-transitive")) {
                            options.flags &= FLAG_RFU_TRANSITIVE;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--show")) {
                            if (arg_id != 1) {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            } else if (arg_id == finalArg_id) {
                                showLongOptionNeedsParametersError(argv[arg_id]);
                                return EXIT_FAILURE;
                            } else {
                                fputs("\n", stderr);
                                for (arg_id++; arg_id < argc; arg_id++)
                                    showDescription(argv[arg_id], descriptionOf_metric(argv[arg_id]));
                                fputs("\n", stderr);
                                return EXIT_SUCCESS;
                            }
                        } else if (STR_CONTAINS_CONST(argv[arg_id], "--show=")) {
                            if (arg_id == 1 && arg_id == finalArg_id) {
                                fputs("\n", stderr);
                                showDescription(argv[arg_id] + 7, descriptionOf_metric(argv[arg_id] + 7));
                                fputs("\n", stderr);
                                return EXIT_SUCCESS;
                            } else {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else if (STR_EQ_CONST(argv[arg_id], "--verbose")) {
                            options.flags |= FLAG_VERBOSE;
                            break;
                        } else if (STR_EQ_CONST(argv[arg_id], "--version")) {
                            if (arg_id == 1 && arg_id == finalArg_id) {
                                showVersion();
                                return EXIT_SUCCESS;
                            } else {
                                showLongOptionMustBeAloneError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else {
                            showUnrecognizedLongOptionError(argv[arg_id]);
                            return EXIT_FAILURE;
                        }
                    case 'a':
                        options.enabledMetrics |= ALL_METRICS_ENABLED;
                        if (argv[arg_id][i + 1] == '\0') {
                            break;
                        } else {
                            goto SRCMETRICS_NEXT_OPTION_CHAR;
                        }
                    case 'c':
                        if (argv[arg_id][i + 1] == '\0' && arg_id == 1 && arg_id == finalArg_id) {
                            showCopyright();
                            return EXIT_SUCCESS;
                        } else {
                            showShortOptionMustBeAloneError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'd':
                        if (argv[arg_id][i + 1] == '=') {
                            csv_delimeter = argv[arg_id] + i + 2;
                            break;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            csv_delimeter = argv[++arg_id];
                            break;
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'e':
                        if (argv[arg_id][i + 1] == '=') {
                            if (enableOrExclude_metric(argv[arg_id] + i + 2, 0)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id] + i + 2);
                                return EXIT_FAILURE;
                            }
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            arg_id++;
                            if (enableOrExclude_metric(argv[arg_id], 0)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'f':
                        if (argv[arg_id][i + 1] == '=') {
                            getEnabledOrExcludedMetricsFromFile(argv[arg_id] + i + 2, 1);
                            break;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            getEnabledOrExcludedMetricsFromFile(argv[++arg_id], 1);
                            break;
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'h':
                        if (argv[arg_id][i + 1] == '\0' && arg_id == 1 && arg_id == finalArg_id) {
                            showLongHelpMessage();
                            return EXIT_SUCCESS;
                        } else {
                            showShortOptionMustBeAloneError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'l':
                        if (argv[arg_id][i + 1] == '=') {
                            options.language = "C";
                            break;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            options.language = "C";
                            arg_id++;
                            break;
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'L':
                        if (argv[arg_id][i + 1] == '\0' && arg_id == 1 && arg_id == finalArg_id) {
                            showListOf_metrics();
                            return EXIT_SUCCESS;
                        } else {
                            showShortOptionMustBeAloneError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'm':
                        if (argv[arg_id][i + 1] == '=') {
                            if (enableOrExclude_metric(argv[arg_id] + i + 2, 1)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id] + i + 2);
                                return EXIT_FAILURE;
                            }
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            arg_id++;
                            if (enableOrExclude_metric(argv[arg_id], 1)) {
                                break;
                            } else {
                                showMetricNOTFoundError(argv[arg_id]);
                                return EXIT_FAILURE;
                            }
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'o':
                        if (argv[arg_id][i + 1] == '=') {
                            options.outfile = argv[arg_id] + i + 2;
                            break;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            options.outfile = argv[++arg_id];
                            break;
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 's':
                        if (argv[arg_id][i + 1] == '=' && arg_id == 1 && arg_id == finalArg_id) {
                            fputs("\n", stderr);
                            showDescription(argv[arg_id] + i + 2, descriptionOf_metric(argv[arg_id] + i + 2));
                            fputs("\n", stderr);
                            return EXIT_SUCCESS;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id == 1) {
                            if (arg_id == finalArg_id) {
                                showShortOptionNeedsParametersError(argv[arg_id][i]);
                                return EXIT_FAILURE;
                            } else {
                                fputs("\n", stderr);
                                for (arg_id++; arg_id < argc; arg_id++)
                                    showDescription(argv[arg_id], descriptionOf_metric(argv[arg_id]));
                                fputs("\n", stderr);
                                return EXIT_SUCCESS;
                            }
                        } else {
                            showShortOptionMustBeAloneError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'v':
                        options.flags |= FLAG_VERBOSE;
                        if (argv[arg_id][i + 1] == '\0') {
                            break;
                        } else {
                            goto SRCMETRICS_NEXT_OPTION_CHAR;
                        }
                    case 'V':
                        if (argv[arg_id][i + 1] == '\0' && arg_id == 1 && arg_id == finalArg_id) {
                            showVersion();
                            return EXIT_SUCCESS;
                        } else {
                            showShortOptionMustBeAloneError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    case 'x':
                        if (argv[arg_id][i + 1] == '=') {
                            getEnabledOrExcludedMetricsFromFile(argv[arg_id] + i + 2, 0);
                            break;
                        } else if (argv[arg_id][i + 1] == '\0' && arg_id < finalArg_id) {
                            getEnabledOrExcludedMetricsFromFile(argv[++arg_id], 0);
                            break;
                        } else {
                            showShortOptionNeedsParametersError(argv[arg_id][i]);
                            return EXIT_FAILURE;
                        }
                    default:
                        showUnrecognizedShortOptionError(argv[arg_id][i]);
                        return EXIT_FAILURE;
                }
                break;
            default:
                if (options.cmd_infiles == NULL) {
                    options.cmd_infiles = malloc(options.cap_cmd_infiles * sizeof(char const*));
                    DEBUG_ERROR_IF(options.cmd_infiles == NULL)
                    DEBUG_ERROR_IF(atexit(free_infiles) != 0)
                    NDEBUG_EXECUTE(atexit(free_infiles))
                }
                REALLOC_IF_NECESSARY(
                    char const*, options.cmd_infiles,
                    size_t, options.cap_cmd_infiles, options.n_cmd_infiles,
                    {REALLOC_ERROR;}
                )
                options.cmd_infiles[options.n_cmd_infiles++] = argv[arg_id];
        }
    }

    /* Register infiles coming from --from-file argument
     * These files were put into the strings chunk. */
    if (options.first_infile_id != 0xFFFFFFFF) {
        for (
            uint32_t infileId = options.first_infile_id;
            infileId <= options.last_infile_id;
            infileId++
        ) {
            char const* infile = get_chunk(strings, infileId);
            DEBUG_ERROR_IF(infile == NULL)

            if (options.cmd_infiles == NULL) {
                options.cmd_infiles = malloc(options.cap_cmd_infiles * sizeof(char const*));
                DEBUG_ERROR_IF(options.cmd_infiles == NULL)
                DEBUG_ERROR_IF(atexit(free_infiles) != 0)
            }

            REALLOC_IF_NECESSARY(
                char const*, options.cmd_infiles,
                size_t, options.cap_cmd_infiles, options.n_cmd_infiles,
                {REALLOC_ERROR;}
            )

            options.cmd_infiles[options.n_cmd_infiles++] = infile;
        }
    }

    if (options.n_cmd_infiles == 0) return EXIT_SUCCESS;

    Chunk chunk[1];
    size_t archiveBufferSize            = 0;
    char* archiveBuffer                 = NULL;
    struct srcml_archive* const archive = srcml_archive_create();
    DEBUG_ASSERT_NDEBUG_EXECUTE(constructEmpty_chunk(chunk, CHUNK_RECOMMENDED_INITIAL_CAP, 1))

    DEBUG_ERROR_IF(srcml_archive_write_open_memory(archive, &archiveBuffer, &archiveBufferSize) != SRCML_STATUS_OK)
    NDEBUG_EXECUTE(srcml_archive_write_open_memory(archive, &archiveBuffer, &archiveBufferSize))
    VERBOSE_MSG_LITERAL("CREATED_EMPTY_SRCML_ARCHIVE");

    for (size_t infile_id = options.n_cmd_infiles - 1; infile_id != SIZE_MAX; infile_id--) {
        char const* const infile = options.cmd_infiles[infile_id];

        VERBOSE_MSG_VARIADIC("SRCML_UNIT = %s", infile);

        /* NOTE: I assume every file contains exactly one unit.
         * This is true for C but maybe not for Java */
        struct srcml_unit* const unit = srcml_unit_create(archive);

        VERBOSE_MSG_LITERAL("SRCML_LANGUAGE = C");

        /* Set language to C */
        DEBUG_ERROR_IF(srcml_unit_set_language(unit, SRCML_LANGUAGE_C) != SRCML_STATUS_OK)
        NDEBUG_EXECUTE(srcml_unit_set_language(unit, SRCML_LANGUAGE_C))

        VERBOSE_MSG_VARIADIC("SRCML_SET_FILENAME = %s", infile);

        /* Set filename */
        DEBUG_ERROR_IF(srcml_unit_set_filename(unit, infile) != SRCML_STATUS_OK)
        NDEBUG_EXECUTE(srcml_unit_set_filename(unit, infile))

        /* Read the unit file */
        FILE* const stream = fopen(infile, "r");
        if (stream == NULL) { showFileNOTFoundError(infile); return EXIT_FAILURE; }

        DEBUG_ERROR_IF(fromStreamAsWhole_chunk(chunk, stream) == 0xFFFFFFFF)
        NDEBUG_EXECUTE(fromStreamAsWhole_chunk(chunk, stream))

        DEBUG_ERROR_IF(fclose(stream) == EOF)
        NDEBUG_EXECUTE(fclose(stream))

        VERBOSE_MSG_VARIADIC("SRCML_UNIT_PARSE => %llu bytes", chunk->len);

        /* Create the unit */
        DEBUG_ERROR_IF(srcml_unit_parse_memory(unit, chunk->start, chunk->len) != SRCML_STATUS_OK)
        NDEBUG_EXECUTE(srcml_unit_parse_memory(unit, chunk->start, chunk->len))

        VERBOSE_MSG_VARIADIC("SRCML_ARCHIVE_WRITE => %s", infile);

        /* Append to the archive */
        DEBUG_ERROR_IF(srcml_archive_write_unit(archive, unit) != SRCML_STATUS_OK)
        NDEBUG_EXECUTE(srcml_archive_write_unit(archive, unit))

        VERBOSE_MSG_VARIADIC("SRCML_FREE => %s", infile);

        /* Copied the unit to the archive, now free the dangling unit */
        srcml_unit_free(unit);

        /* Flush the chunk */
        DEBUG_ASSERT_NDEBUG_EXECUTE(flush_chunk(chunk))
    }

    /* Free the chunk */
    DEBUG_ASSERT_NDEBUG_EXECUTE(free_chunk(chunk))

    /* Close the archive */
    srcml_archive_close(archive);

    /* Free the archive */
    srcml_archive_free(archive);

    /* Second Task: Do srcsax stuff on the archive */
    struct srcsax_context* context = srcsax_create_context_memory(archiveBuffer, archiveBufferSize, NULL);
    DEBUG_ERROR_IF(context == NULL)

    /* VERY IMPORTANT, DO NOT FORGET */
    context->handler = getStaticEventHandler();

    VERBOSE_MSG_LITERAL("SRCSAX_CONTEXT_CREATED");

    DEBUG_ERROR_IF(srcsax_parse(context) == -1)
    NDEBUG_EXECUTE(srcsax_parse(context))

    VERBOSE_MSG_LITERAL("SRCSAX_PARSE_COMPLETED");

    DEBUG_ASSERT_NDEBUG_EXECUTE(reportCsv())

    VERBOSE_MSG_LITERAL("REPORT_CSV_COMPLETED");

    srcsax_free_context(context);

    return EXIT_SUCCESS;
}
