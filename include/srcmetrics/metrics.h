/**
 * @file metrics.h
 * @brief Defines all metrics. It supports up to 64 metrics.
 * @author Yavuz Koroglu
 */
#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include <stddef.h>

    #define NOT_A_METRIC_ID             ALL_METRICS_ENABLED
    #define DESCRIPTION_OF_NOT_A_METRIC "Unknown Metric"
    #define METRICS_COUNT_MAX           64

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
    bool enableOrExclude_metric(char const* const restrict abbreviation, bool const enable);
#endif
