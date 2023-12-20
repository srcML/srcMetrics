/**
 * @file metrics.h
 * @brief Defines all metrics. It supports up to 64 metrics.
 * @author Yavuz Koroglu
 * @see metrics.c
 */
#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include <stddef.h>

    #define NOT_A_METRIC_ID             METRICS_COUNT_MAX + 1
    #define DESCRIPTION_OF_NOT_A_METRIC "Unknown Metric"
    #define METRICS_COUNT_MAX           64

    #define METRICS {                                                           \
        "ABC",                                                                  \
        "AMS",                                                                  \
        "CC",                                                                   \
        "HSM",                                                                  \
        "MC",                                                                   \
        "MND",                                                                  \
        "NPM",                                                                  \
        "RFU",                                                                  \
        "SLOC",                                                                 \
        NULL                                                                    \
    }
    #define METRIC_DESCRIPTIONS {                                               \
        "√(A²+B²+C²);\n"                                                        \
            "            A: Assignments,\n"                                     \
            "            B: Branches,\n"                                        \
            "            C: Conditionals",                                      \
        "Average Method Size",                                                  \
        "Cyclomatic Complexity (needs Control-Flow Graph)",                     \
        "Halstead Software Metrics;\n"                                          \
            "            D: Difficulty,\n"                                      \
            "            V: Volume,\n"                                          \
            "            E: Effort,\n"                                          \
            "            B: Expected Number of Bugs,\n"                         \
            "            T: Time to Code (workdays)",                           \
        "Method Count",                                                         \
        "Maximum Nesting Depth",                                                \
        "Number of Public Methods",                                             \
        "Response for Unit",                                                    \
        "Source Lines of Code",                                                 \
        NULL                                                                    \
    }

    /**
     * @brief Given its abbreviation, returns the index of a metric.
     * @param abbreviation The metric abbreviation.
     * @return NOT_A_METRIC_ID if the metric does NOT exist, the metric index otherwise.
     */
    size_t indexOf_metric(char const* const abbreviation);

    /**
     * @brief Given its abbreviation, returns the description of a metric.
     * @param abbreviation The metric abbreviation.
     * @return DESCRIPTION_OF_NOT_A_METRIC if the metric does NOT exist, the metric description otherwise.
     */
    char const* descriptionOf_metric(char const* const abbreviation);

    /**
     * @brief Reports the list of all supported metrics.
     */
    void showListOf_metrics(void);

    /**
     * @brief Checks if a given metric index is valid.
     * @param metricId The metric index.
     * @return 0 if metridId is invalid, 1 otherwise.
     */
    bool isValidIndex_metric(size_t const metricId);

    /**
     * @brief Given its abbreviation, enables or excludes a metric.
     * @param abbreviation The metric abbreviation.
     * @param enable Enables the metric if 1, excludes otherwise.
     * @return 0 if the metric does NOT exist, 1 otherwise.
     */
    bool enableOrExclude_metric(char const* const abbreviation, bool const enable);
#endif
