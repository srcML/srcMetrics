/**
 * @file metrics.h
 * @brief Defines all metrics. It supports up to 64 metrics.
 * @author Yavuz Koroglu
 */
#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include <stddef.h>
    #include <unistd.h>
    #include "util/bliterals.h"

    #define NOT_A_METRIC -1

    #define DESCRIPTION_OF_NOT_A_METRIC "Unknown Metric"

    #define ENABLE_ALL_METRICS B8(B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111)

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

    ssize_t indexOf_metric(char const* const restrict abbreviation);
    char const* descriptionOf_metric(char const* const restrict abbreviation);
    void showListOf_metrics(void);
#endif
