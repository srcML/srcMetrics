#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include "util/bliterals.h"

    #define METRIC_MAX_NAME_SIZE        10
    #define METRIC_MAX_DESCRIPTION_SIZE 8000

    #define METRIC_NPM 0
    #define METRIC_SLOC 1

    #define METRICS {                   \
        "NPM",                          \
        "SLOC",                         \
        NULL                            \
    }
    #define METRIC_DESCRIPTIONS {       \
        "Number of Public Methods",     \
        "Source Lines of Code",         \
        NULL                            \
    }

    #define ENABLE_ALL_METRICS B8(B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111)
#endif
