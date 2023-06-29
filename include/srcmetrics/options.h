/**
 * @file options.h
 * @brief Defines Options and the initial options before command-line arguments.
 *
 * @author Yavuz Koroglu
 */
#ifndef OPTIONS_H
    #define OPTIONS_H
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdio.h>
    #include "libsrcml/srcml.h"
    #include "srcmetrics/metrics.h"

    /**
     * @def OPTIONS_INITIAL
     *   Initial options are no infiles with BUFSIZ capacity, no outfile, no language, standard out, and all metrics enabled.
     */
    #define OPTIONS_INITIAL ((Options){ 0, BUFSIZ, NULL, NULL, SRCML_LANGUAGE_NONE, stdout, ENABLE_ALL_METRICS })

    /**
     * @struct Options
     * @brief srcMetrics Options.
     *
     * Command-line parameters usually interact with these options.
     *
     * @see main()
     */
    typedef struct OptionsBody {
        size_t infiles_count;
        size_t infiles_cap;
        char** infiles;
        char const* outfile;
        char const* language;
        FILE* statusOutput;
        uint64_t metrics;
    } Options;
#endif
