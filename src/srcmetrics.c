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
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "libsrcml/srcml.h"
#include "srcmetrics/event.h"
#include "srcmetrics/metrics.h"
#include "srcmetrics/options.h"
#include "srcmetrics/report.h"
#include "util/csv.h"
#include "util/readfile.h"
#include "util/streq.h"
#include "util/unless.h"
#include "util/until.h"

char const* csv_delimeter   = CSV_INITIAL_DELIMETER;
char const* csv_row_end     = CSV_INITIAL_ROW_END;
Chunk strings               = NOT_A_CHUNK;
struct Options options;

/**
 * @defgroup atexit_Functions Functions Called @ Exit
 * @{
 */

/**
 * @brief Puts a final newline to the standard error before termination.
 *
 * Putting a newline into a standard buffer may ensure a final flush in some systems.
 * Also, it looks nice on a terminal window.
 */
static void putFinalNewLine(void)   { fputs("\n", stderr); }

/**
 * @brief Frees the infiles array from the options.
 * @see Options::infiles
 */
static void free_infiles(void)      { free(options.infiles); }

/**
 * @brief Whatever string main() allocated, free them.
 */
static void free_strings(void)      { free_chunk(strings); }

/**@}*/

/**
 * @defgroup Message_Functions Message Functions
 * @{
 */

/**
 * @brief Prints the copyright message.
 */
static void showCopyrightMessage(void) {
    fputs("Copyright (C) 2023 srcML, LLC. (www.srcML.org)\n", stderr);
}

/**
 * @brief Prints the long help message.
 */
static void showLongHelpMessage(void) {
    fputs("Usage: srcmetrics [options] <src_infile>... [-o <srcMetrics_outfile>]\n", stderr);
    fputs("       srcmetrics [options] <srcML_infile>... [-o <srcMetrics_outfile>]\n", stderr);
    fputs("       srcmetrics [options] <srcGraph_infile>... [-o <srcMetrics_outfile>]\n", stderr);
    fputs("\n", stderr);
    fputs("Calculates static metrics from C source code, srcML, or srcGraph files\n", stderr);
    fputs("\n", stderr);
    fputs("Source-code input can be from standard input, a file, a directory, or an archive file, i.e., tar, cpio, and zip.\n", stderr);
    fputs("\n", stderr);
    fputs("GENERAL OPTIONS:\n", stderr);
    fputs("  -h,--help                  Output this help message and exit\n", stderr);
    fputs("  -V,--version               Output version number and exit\n", stderr);
    fputs("  -v,--verbose               Status information to stderr\n", stderr);
    fputs("  -c,--copyright             Output the copyright message and exit\n", stderr);
    fputs("  -q,--quiet                 Suppress status messages\n", stderr);
    fputs("  -o,--output FILE           Write output to FILE\n", stderr);
    fputs("  -l,--language LANG         Set the source-code language to C\n", stderr);
    fputs("  -d,--delimeter DELIM       Change the CSV delimeter, default: ','\n", stderr);
    fputs("  --files-from FILE          Input source-code filenames from FILE instead of command-line arguments\n", stderr);
    /*fputs("  --src-encoding ENCODING    Set the input source-code encoding\n", stderr);*/
    fputs("\n", stderr);
    fputs("SRCMETRICS:\n", stderr);
    fputs("  -m,--metric METRIC         Report this METRIC if possible, excludes all unspecified metrics\n", stderr);
    fputs("  -e,--exclude METRIC        Exclude this METRIC from the output\n", stderr);
    fputs("  -f,--metrics-from FILE     Input enabled metrics from FILE\n", stderr);
    fputs("  -x,--excluded-from FILE    Input exclided metrics from FILE\n", stderr);
    fputs("\n", stderr);
    fputs("METADATA OPTIONS:\n", stderr);
    fputs("  -L,--list                  Output the list of supported metrics and exit\n", stderr);
    fputs("  -s,--show METRIC           Output detailed information on METRIC and exit\n", stderr);
    fputs("\n", stderr);
    fputs("Have a question or need to report a bug?\n", stderr);
    fputs("Contact us at www.srcml.org/support.html\n", stderr);
    fputs("www.srcML.org\n", stderr);
}

/**
 * @brief Prints the short help message.
 */
static void showShortHelpMessage(void) {
    fputs("srcmetrics typically accepts input from pipe, not a terminal\n", stderr);
    fputs("Typical usage includes:\n", stderr);
    fputs("\n", stderr);
    fputs("    # compute all non-graph metrics of a source file and save it to standard out\n", stderr);
    fputs("    srcmetrics main.c\n", stderr);
    fputs("\n", stderr);
    fputs("    # compute only Cyclomatic Complexity of a source file and save it to a report file\n", stderr);
    fputs("    srcmetrics main.c -m CC -o report.csv\n", stderr);
    fputs("\n", stderr);
    fputs("See `srcmetrics --help` for more information\n", stderr);
}

/**
 * @brief Prints the program and library versions.
 */
static void showVersion(void) {
    fputs("srcmetrics "VERSION_SRCMETRICS"\n", stderr);
    fprintf(stderr, "libsrcml %s\n", srcml_version_string()); /*, srcml_version_number());*/
}

/**@}*/

/**
 * @brief Gets infiles using the file given with '--files-from' argument.
 *
 * WARNING: Does NOT check if valid file name (e.g. '>' and '&' characters)!!
 *
 * @param filename The '--files-from' file name.
 * @return 1 if it can get the infiles from file, 0 otherwise.
 */
static bool getInfilesFromFile(char const* const restrict filename) {
    unless (isValid_chunk(strings)) {
        strings = constructEmpty_chunk(BUFSIZ);
        atexit(free_strings);
    }
    unless (isValid_chunk(strings = open_readChunk_close(strings, filename))) return 0;

    {
        char* true_start = strings.start;
        while (true_start < strings.end && isspace(*true_start)) true_start++;
        unless (true_start < strings.end) return 0;

        unless (options.infiles) {
            options.infiles = malloc(options.infiles_cap * sizeof(char*));
            atexit(free_infiles);
        }

        options.infiles[options.infiles_count++] = true_start;
        for (char* ptr = true_start; ptr < strings.end;) {
            unless (isspace(*ptr))  { ptr++; continue; }
            while (isspace(*ptr))   { *(ptr++) = '\0'; }
            unless (options.infiles_count < options.infiles_cap) {
                unless (
                    (options.infiles_cap <<= 1) > options.infiles_count &&
                    (options.infiles = realloc(options.infiles, options.infiles_cap * sizeof(char*)))
                ) return 0;
            }
            options.infiles[options.infiles_count++] = ptr;
        }
    }

    return 1;
}

/**
 * @brief Gets enabled or excluded metrics using the file given with '--metrics-from' argument.
 *
 * WARNING: Does NOT check if valid file name (e.g. '>' and '&' characters)!!
 *
 * @param filename The '--metrics-from' file name.
 * @param enable Enable metrics if 1, exclude otherwise.
 * @return 1 if it can get the metrics from file, 0 otherwise.
 */
static bool getEnabledOrExcludedMetricsFromFile(char const* const restrict filename, bool const enable) {
    unless (isValid_chunk(strings)) {
        strings = constructEmpty_chunk(BUFSIZ);
        atexit(free_strings);
    }
    unless (isValid_chunk(strings = open_readChunk_close(strings, filename))) return 0;

    {
        char* true_start = strings.start;
        while (true_start < strings.end && isspace(*true_start)) true_start++;
        unless (true_start < strings.end) return 0;

        for (char* metric = true_start; metric < strings.end;) {
            unless (isspace(*metric))   { metric++; continue; }
            while (isspace(*metric))    { metric++; }
            enableOrExclude_metric(metric, enable);
        }
    }
    return 1;
}

/**
 * @brief Parses the command-line arguments and starts the metrics collection.
 * @param argc #arguments including the program name.
 * @param argv The list of arguments as an array of strings.
 * @return EXIT_FAILURE if something goes wrong, EXIT_SUCCESS otherwise.
 */
int main(int argc, char* argv[]) {
    char** const finalArg = argv + argc - 1;

    options = OPTIONS_INITIAL;

    fputs("\n", stderr);
    atexit(putFinalNewLine);

    if (argc < 2) {
        showShortHelpMessage();
        return EXIT_SUCCESS;
    }

    /* Process all arguments */
    for (char** arg = argv + 1; arg <= finalArg; arg++) {
        /* Check if this is an option argument */
        if (**arg == '-') {
            /* Short option format */

            /* Assume the dash continues */
            bool dash_continues = 1;

            /* Process all options */
            for (char const* option = *arg + 1; dash_continues && *option && !isspace(*option); option++) {
                switch (*(option)) {
                    case 'h':
                        showLongHelpMessage();
                        return EXIT_SUCCESS;
                    case 'V':
                        showVersion();
                        return EXIT_SUCCESS;
                    case 'c':
                        showCopyrightMessage();
                        return EXIT_SUCCESS;
                    case 'l':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            options.language = option;
                        } else unless (arg == finalArg) {
                            options.language = *(++arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'o':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            options.outfile = option;
                        } else unless (arg == finalArg) {
                            options.outfile = *(++arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'q':
                        options.statusOutput = NULL;
                        break;
                    case 'v':
                        options.statusOutput = stderr;
                        break;
                    case 's':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            fprintf(stderr, "%s: %s\n", option, descriptionOf_metric(option));
                        } else unless (arg == finalArg) {
                            arg++;
                            fprintf(stderr, "%s: %s\n", *arg, descriptionOf_metric(*arg));
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                            break;
                        }
                        return EXIT_SUCCESS;
                    case 'L':
                        showListOf_metrics();
                        return EXIT_SUCCESS;
                    case 'm':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            unless (enableOrExclude_metric(option, 1))
                                fprintf(stderr, "Unknown Metric: %s\n", option);
                        } else unless (arg == finalArg) {
                            unless (enableOrExclude_metric(*(++arg), 1))
                                fprintf(stderr, "Unknown Metric: %s\n", *arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'e':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            unless (enableOrExclude_metric(option, 0))
                                fprintf(stderr, "Unknown Metric: %s\n", option);
                        } else unless (arg == finalArg) {
                            unless (enableOrExclude_metric(*(++arg), 0))
                                fprintf(stderr, "Unknown Metric: %s\n", *arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'f':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            unless (getEnabledOrExcludedMetricsFromFile(option, 1))
                                fprintf(stderr, "Could NOT get enabled metrics from '%s'\n", option);
                        } else unless (arg == finalArg) {
                            unless (getEnabledOrExcludedMetricsFromFile(*(++arg), 1))
                                fprintf(stderr, "Could NOT get enabled metrics from '%s'\n", *arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'x':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            unless (getEnabledOrExcludedMetricsFromFile(option, 0))
                                fprintf(stderr, "Could NOT get exclude metrics from '%s'\n", option);
                        } else unless (arg == finalArg) {
                            unless (getEnabledOrExcludedMetricsFromFile(*(++arg), 0))
                                fprintf(stderr, "Could NOT get exclude metrics from '%s'\n", *arg);
                        } else {
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        }
                        break;
                    case 'd':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option))
                            csv_delimeter = option;
                        else unless (arg == finalArg)
                            csv_delimeter = *(++arg);
                        else
                            fprintf(stderr, "There is something wrong in argument = %s\n", *arg);
                        break;
                    case '-':
                        /* Long option format */
                        dash_continues = 0;
                        if (str_eq_const(++option, "help")) {
                            showLongHelpMessage();
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "version")) {
                            showVersion();
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "copyright")) {
                            showCopyrightMessage();
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "quiet")) {
                            options.statusOutput = NULL;
                        } else if (str_eq_const(option, "verbose")) {
                            options.statusOutput = stderr;
                        } else if (str_eq_const(option, "language=")) {
                            options.language = option + sizeof("language");
                        } else if (str_eq_const(option, "output=")) {
                            options.outfile = option + sizeof("output");
                        } else if (str_eq_const(option, "files-from=")) {
                            unless (getInfilesFromFile((option + sizeof("files-from")))) {
                                fprintf(stderr, "Could NOT get infiles from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "show=")) {
                            option += sizeof("show");
                            fprintf(stderr, "%s: %s\n", option, descriptionOf_metric(option));
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "metric=")) {
                            enableOrExclude_metric(option + sizeof("metric"), 1);
                        } else if (str_eq_const(option, "exclude=")) {
                            enableOrExclude_metric(option + sizeof("exclude"), 0);
                        } else if (str_eq_const(option, "metrics-from=")) {
                            unless (getEnabledOrExcludedMetricsFromFile(option + sizeof("metrics-from"), 1)) {
                                fprintf(stderr, "Could NOT get enabled metrics from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "excluded-from=")) {
                            unless (getEnabledOrExcludedMetricsFromFile(option + sizeof("exclude-from"), 0)) {
                                fprintf(stderr, "Could NOT get enabled metrics from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "delimeter=")) {
                            csv_delimeter = option + sizeof("delimeter");
                        } else if (str_eq_const(option, "language") && arg != finalArg) {
                            options.language = *(++arg);
                        } else if (str_eq_const(option, "output") && arg != finalArg) {
                            options.outfile = *(++arg);
                        } else if (str_eq_const(option, "files-from") && arg != finalArg) {
                            unless (getInfilesFromFile(*(++arg))) {
                                fprintf(stderr, "Could NOT get infiles from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "show")) {
                            arg++;
                            fprintf(stderr, "%s: %s\n", *arg, descriptionOf_metric(*arg));
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "list")) {
                            showListOf_metrics();
                            return EXIT_SUCCESS;
                        } else if (str_eq_const(option, "metric")) {
                            unless (enableOrExclude_metric(*(++arg), 1))
                                fprintf(stderr, "Unknown Metric: %s\n", *arg);
                        } else if (str_eq_const(option, "exclude")) {
                            unless (enableOrExclude_metric(*(++arg), 0))
                                fprintf(stderr, "Unknown Metric: %s\n", *arg);
                        } else if (str_eq_const(option, "metrics-from")) {
                            unless (getEnabledOrExcludedMetricsFromFile(*(++arg), 1)) {
                                fprintf(stderr, "Could NOT get enabled metrics from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "excluded-from")) {
                            unless (getEnabledOrExcludedMetricsFromFile(*(++arg), 0)) {
                                fprintf(stderr, "Could NOT get excluded metrics from '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "delimeter") && arg != finalArg) {
                            csv_delimeter = *(++arg);
                        } else {
                            fprintf(stderr, "Could NOT understand long option = %s\n", *arg);
                        }
                        break;
                    default:
                        fprintf(stderr, "Unknown option '%c' in argument = %s\n", *(option), *arg);
                        break;
                }
            }
        } else {
            /* Default Argument: Input Files */

            /* Cannot declare files both from argv and 'files-from' */
            for (char** arg2 = argv + 1; arg2 <= finalArg; arg2++) {
                unless (str_eq_const(*arg2, "–-files-from") && arg != arg2) continue;
                showLongHelpMessage();
                return EXIT_FAILURE;
            }

            unless (options.infiles) {
                options.infiles = malloc(options.infiles_cap * sizeof(char*));
                atexit(free_infiles);
            }

            unless (options.infiles_count < options.infiles_cap) {
                unless (
                    (options.infiles_cap <<= 1) > options.infiles_count &&
                    (options.infiles = realloc(options.infiles, options.infiles_cap * sizeof(char*)))
                ) { fputs("Could NOT get infiles\n", stderr); return EXIT_FAILURE; };
            }

            options.infiles[options.infiles_count++] = *arg;
        }
    }

    /* This code is for trial and should be deleted */
    {
        /* First Task: Create an archive in the memory */
        size_t archiveBufferSize = 0U;
        char* archiveBuffer = NULL;
        struct srcml_archive* archive = srcml_archive_create();
        unless (srcml_archive_write_open_memory(archive, &archiveBuffer, &archiveBufferSize) == SRCML_STATUS_OK) {
            fputs("Cannot create archive\n", stderr);
            return EXIT_FAILURE;
        }

        /* Parse all units into an archive */
        {
            char* unitBuffer = malloc(131072U);
            /* For every file */
            for (char** infile = options.infiles + options.infiles_count - 1; infile >= options.infiles; infile--) {
                int srcml_fd;
                ssize_t nBytes;
                struct srcml_unit* unit = srcml_unit_create(archive);
                /* NOTE: I assume every file contains exactly one unit. This is true for C but maybe not for Java */

                /* Open the input source code */
                if ((srcml_fd = open(*infile, O_RDONLY, 0)) == -1) { fprintf(stderr, "Could NOT open %s\n", *infile); return EXIT_FAILURE; }

                /* Read the source code into the unit buffer */
                if ((nBytes = read(srcml_fd, unitBuffer, 131071U)) == -1) { fputs("Read Error\n", stderr); return EXIT_FAILURE; }

                /* Just a size check */
                if (nBytes >= 131070) { fprintf(stderr, "%s is too big to read (%zu bytes)\n", *infile, (size_t)nBytes); return EXIT_FAILURE; }

                fprintf(stderr, "Just read %zu bytes\n", (size_t)nBytes);

                /* Close the file */
                if (close(srcml_fd) == -1) { fprintf(stderr, "Cannot close %s\n", *infile); return EXIT_FAILURE; }

                /* Set language to C */
                unless (srcml_unit_set_language(unit, SRCML_LANGUAGE_C) == SRCML_STATUS_OK) { fputs("Cannot set language\n", stderr); return EXIT_FAILURE; }

                /* Set filename */
                unless (srcml_unit_set_filename(unit, *infile) == SRCML_STATUS_OK) { fputs("Cannot set file\n", stderr); return EXIT_FAILURE; }

                /* Create the unit */
                unless (srcml_unit_parse_memory(unit, unitBuffer, (size_t)nBytes) == SRCML_STATUS_OK) { fputs("Cannot parse unit\n", stderr); return EXIT_FAILURE; }

                /* Append to the archive */
                /* Q: Did I copy the unit by doing this? */
                unless (srcml_archive_write_unit(archive, unit) == SRCML_STATUS_OK) { fputs("Cannot write unit to archive\n", stderr); return EXIT_FAILURE; }

                /* If I copied the unit to the archive, I should free the dangling unit */
                srcml_unit_free(unit);
            }
            free(unitBuffer);
        }

        /* Close the archive */
        srcml_archive_close(archive);

        /* Free the archive */
        srcml_archive_free(archive);

        fprintf(stderr, "archiveBufferSize = %zu bytes\n\n", archiveBufferSize);

        /* Second Task: Do srcsax stuff on the archive */
        {
            struct srcsax_handler* events = getStaticEventHandler();
            struct srcsax_context* context = srcsax_create_context_memory(archiveBuffer, archiveBufferSize, NULL);
            unless (context) { fputs("SRCSAX_ERROR\n", stderr); return EXIT_FAILURE; }

            /* VERY IMPORTANT, DO NOT FORGET */
            context->handler = events;

            if (srcsax_parse(context) == -1) { fputs("PARSE_ERROR\n", stderr); return EXIT_FAILURE; }

            unless (reportCsv()) { fputs("REPORT_ERROR\n", stderr); return EXIT_FAILURE; }

            srcsax_free_context(context);
        }
    }

    return EXIT_SUCCESS;
}
