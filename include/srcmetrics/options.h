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

    /**
     * @def OPTIONS_INITIAL
     *   Initial options are no infiles with BUFSIZ capacity, no outfile, no language, and standard out
     */
    #define OPTIONS_INITIAL ((Options){ 0, BUFSIZ, NULL, NULL, SRCML_LANGUAGE_NONE, stdout })

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
    } Options;
#endif
