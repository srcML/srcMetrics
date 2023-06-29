/**
 * @file metrics.h
 * @brief Defines all metrics. It supports up to 64 metrics.
 * @author Yavuz Koroglu
 */
#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>
    #include "util/bliterals.h"

    #define ALL_METRICS_ENABLED         B8(B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111,B_11111111)
    #define NOT_A_METRIC_ID             ALL_METRICS_ENABLED
    #define DESCRIPTION_OF_NOT_A_METRIC "Unknown Metric"

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

    size_t indexOf_metric(char const* const restrict abbreviation);
    char const* descriptionOf_metric(char const* const restrict abbreviation);
    void showListOf_metrics(void);
    bool isValidIndex_metric(size_t const metricId);
    bool enable_metric(uint_fast64_t* const restrict enabledMetrics, char const* const restrict abbreviation);
    bool exclude_metric(uint_fast64_t* const restrict enabledMetrics, char const* const restrict abbreviation);
#endif
