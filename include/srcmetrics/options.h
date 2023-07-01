/**
 * @file options.h
 * @brief Defines Options and the initial options before command-line arguments.
 * @author Yavuz Koroglu
 */
#ifndef OPTIONS_H
    #define OPTIONS_H
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include "util/bliterals.h"

    /**
     * @struct Options
     * @brief Global srcMetrics Options.
     *
     * Command-line parameters usually interact with these options.
     *
     * @see srcmetrics.c
     */
    extern struct Options {
        size_t infiles_count;
        size_t infiles_cap;
        char** infiles;
        char const* outfile;
        char const* language;
        FILE* statusOutput;
        uint_fast64_t enabledMetrics;
    } options;

    #define ALL_METRICS_ENABLED B8(B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111)

    /**
     * @def OPTIONS_INITIAL
     *   Initial options are no infiles with BUFSIZ capacity, no outfile, no language, standard out, and all metrics enabled.
     */
    #define OPTIONS_INITIAL ((struct Options){ 0, BUFSIZ, NULL, NULL, SRCML_LANGUAGE_NONE, stdout, ALL_METRICS_ENABLED })
#endif
