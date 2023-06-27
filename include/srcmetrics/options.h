#ifndef OPTIONS_H
    #define OPTIONS_H
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdio.h>

    #define OPTIONS_INITIAL ((Options){ 0, BUFSIZ, NULL, NULL, NULL, stdout })

    typedef struct OptionsBody {
        size_t infiles_count;
        size_t infiles_cap;
        char** infiles;
        char const* outfile;
        char const* language;
        FILE* statusOutput;
    } Options;
#endif
