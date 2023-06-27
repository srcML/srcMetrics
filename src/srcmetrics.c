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
 * * Obtaining static metrics from the source code and its control-flow/call graphs.
 */
#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

static void startDocument(struct srcsax_context* context) {
    unless (context) exit(EXIT_FAILURE);
}
static void endDocument(struct srcsax_context* context) {
    unless (context) exit(EXIT_FAILURE);
}
static void startRoot(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI, int num_namespaces, struct srcsax_namespace const* namespaces, int num_attributes, struct srcsax_attribute const* attributes) {
    unless (context) exit(EXIT_FAILURE);
}
static void startUnit(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI, int num_namespaces, struct srcsax_namespace const* namespaces, int num_attribute, struct srcsax_attribute const* attributes) {
    unless (context) exit(EXIT_FAILURE);
}
static void startElement(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI, int num_namespaces, struct srcsax_namespace const* namespaces, int num_attributes, struct srcsax_attribute const* attributes) {
    unless (context) exit(EXIT_FAILURE);
}
static void endRoot(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI) {
    unless (context) exit(EXIT_FAILURE);
}
static void endUnit(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI) {
    unless (context) exit(EXIT_FAILURE);
}
static void endElement(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI) {
    unless (context) exit(EXIT_FAILURE);
}
static void charactersRoot(struct srcsax_context* context, char const* ch, int len) {
    unless (context) exit(EXIT_FAILURE);
}
static void charactersUnit(struct srcsax_context* context, char const* ch, int len) {
    unless (context) exit(EXIT_FAILURE);
}
static void metaTag(struct srcsax_context* context, char const* localname, char const* prefix, char const* URI, int num_namespaces, struct srcsax_namespace const* namespaces, int num_attributes, struct srcsax_attribute const* attributes) {
    unless (context) exit(EXIT_FAILURE);
}
static void comment(struct srcsax_context* context, char const* value) {
    unless (context) exit(EXIT_FAILURE);
}
static void cdataBlock(struct srcsax_context * context, char const* value, int len) {
    unless (context) exit(EXIT_FAILURE);
}
static void procInfo(struct srcsax_context* context, char const* target, char const* data) {
    unless (context) exit(EXIT_FAILURE);
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

                fprintf(stderr, "INFILE = %s\n\n", *infile);

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

        fprintf(stderr, "archiveBufferSize = %zu bytes\n", archiveBufferSize);

        /* Second Task: Do srcsax stuff on the archive */
        {
            struct srcsax_handler handler = {
                startDocument, endDocument,
                startRoot, startUnit, startElement,
                endRoot, endUnit, endElement,
                charactersRoot, charactersUnit,
                metaTag, comment, cdataBlock, procInfo
            };
            struct srcsax_context* context = srcsax_create_context_memory(archiveBuffer, archiveBufferSize, NULL);
            unless (context) { fputs("SRCSAX Error\n", stderr); return EXIT_FAILURE; }

            /* VERY IMPORTANT, DO NOT FORGET */
            context->handler = &handler;

            if (srcsax_parse(context) == -1) { fputs("PARSE ERROR\n", stderr); return EXIT_FAILURE; }

            fprintf(stderr, "UNIT COUNT = %d\n", context->unit_count);
            fprintf(stderr, "STACK SIZE = %zu\n", context->stack_size);
            fprintf(stderr, "IS_ARCHIVE = %s\n", context->is_archive ? "YES" : "NO");

            srcsax_free_context(context);
        }
    }

    return EXIT_SUCCESS;
}
