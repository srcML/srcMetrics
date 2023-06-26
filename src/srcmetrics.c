/**
 * @file srcmetrics.c
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
 * @mainpage srcmetrics
 *
 * The functions in srcmetrics are for the purposes of:
 *
 * * Obtaining static metrcis from the source code and its control-flow/call graphs.
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern const unsigned int SRCML_OPTION_NO_XML_DECL;
extern const unsigned int SRCML_OPTION_POSITION;
extern const unsigned int SRCML_OPTION_CPP;
extern const unsigned int SRCML_OPTION_CPP_TEXT_ELSE;
extern const unsigned int SRCML_OPTION_CPP_MARKUP_IF0;
extern const unsigned int SRCML_OPTION_STORE_ENCODING;
#include "libsrcml/srcml.h"
#include "libsrcsax/srcsax.h"
#include "libsrcsax/srcsax_handler.h"

#include "srcmetrics/options.h"
#include "srcmetrics/version.h"
#include "util/chunk.h"
#include "util/streq.h"
#include "util/unless.h"
#include "util/until.h"

static Options options;
static Chunk strings;

/* Functions called @ exit */
static void putFinalNewLine(void)   { fputs("\n", stderr); }
static void free_infiles(void)      { free(options.infiles); }
static void free_strings(void)      { free_chunk(&strings); }

/* Message functions */
static void showCopyrightMessage(void) {
    fputs("Copyright (C) 2023 srcML, LLC. (www.srcML.org)\n", stderr);
}
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
    fputs("  --files-from FILE          Input source-code filenames from FILE instead of command-line arguments\n", stderr);
    /*fputs("  --src-encoding ENCODING    Set the input source-code encoding\n", stderr);*/
    fputs("\n", stderr);
    fputs("SRCMETRICS:\n", stderr);
    fputs("  -m,--metric METRIC         Report this METRIC if possible, excludes all unspecified metrics\n", stderr);
    fputs("  -e,--exclude METRIC        Exclude this METRIC from the output\n", stderr);
    fputs("\n", stderr);
    fputs("METADATA OPTIONS:\n", stderr);
    fputs("  -L,--list                  Output the list of supported metrics and exit\n", stderr);
    fputs("  -s,--show METRIC           Output detailed information on METRIC and exit\n", stderr);
    fputs("\n", stderr);
    fputs("Have a question or need to report a bug?\n", stderr);
    fputs("Contact us at www.srcml.org/support.html\n", stderr);
    fputs("www.srcML.org\n", stderr);
}
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
static void showVersion(void) {
    fputs("srcmetrics "VERSION_SRCMETRICS"\n", stderr);
    fprintf(stderr, "libsrcml %s\n", srcml_version_string()); /*, srcml_version_number());*/
}

/* Gets infiles using the file given with '--files-from' argument' */
static bool getInfilesFromFile(char const* const restrict filename) {
    char infile[BUFSIZ] = {0};
    FILE* file = fopen(filename, "r"); unless (file) return 0;
    {
        strings = constructEmpty_chunk(BUFSIZ);
        atexit(free_strings);

        unless (options.infiles) {
            options.infiles = malloc(options.infiles_cap * sizeof(char*));
            atexit(free_infiles);
        }

        while (fscanf(file, "%s", infile)) {
            /* File name is too long! */
            unless (infile[BUFSIZ-1] == '\0') return 0;

            unless (options.infiles_count < options.infiles_cap)
                options.infiles = realloc(options.infiles, (options.infiles_cap <<= 1) * sizeof(char const*));

            options.infiles[options.infiles_count++] = add_chunk(&strings, infile, strlen(infile));
        }
    }
    fclose(file);
    return 1;
}

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
                        }
                        break;
                    case 'o':
                        dash_continues = 0;
                        if (*(++option) == '=' && *(++option)) {
                            options.outfile = option;
                        } else unless (arg == finalArg) {
                            options.outfile = *(++arg);
                        }
                        break;
                    case 'q':
                        options.statusOutput = NULL;
                    case 'v':
                        options.statusOutput = stderr;
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
                            unless (getInfilesFromFile((option + sizeof("files-from=")))) {
                                fprintf(stderr, "Could NOT open '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
                        } else if (str_eq_const(option, "language") && arg != finalArg) {
                            options.language = *(++arg);
                        } else if (str_eq_const(option, "output") && arg != finalArg) {
                            options.outfile = *(++arg);
                        } else if (str_eq_const(option, "files-from") && arg != finalArg) {
                            unless (getInfilesFromFile(*(++arg))) {
                                fprintf(stderr, "Could NOT open '%s'\n", *arg);
                                return EXIT_FAILURE;
                            }
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
                options.infiles = realloc(options.infiles, (options.infiles_cap <<= 1) * sizeof(char const*));
            }

            options.infiles[options.infiles_count++] = *arg;
        }
    }

    return EXIT_SUCCESS;
}
