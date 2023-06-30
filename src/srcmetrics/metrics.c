/**
 * @file metrics.c
 * @brief Implements the functions defined in metrics.h
 * @author Yavuz Koroglu
 * @see metrics.h
 */
#include <stdio.h>
#include <string.h>

#include "srcmetrics/metrics.h"
#include "srcmetrics/options.h"
#include "util/unless.h"

static char const* metrics[] = METRICS;
static char const* metric_descriptions[] = METRIC_DESCRIPTIONS;

size_t indexOf_metric(char const* const restrict abbreviation) {
    unless (abbreviation) return NOT_A_METRIC_ID;
    for (size_t metricId = 0; metrics[metricId]; metricId++) {
        unless (!strncmp(abbreviation, metrics[metricId], strlen(metrics[metricId]))) continue;
        return metricId;
    }
    return NOT_A_METRIC_ID;
}

char const* descriptionOf_metric(char const* const restrict abbreviation) {
    size_t const index = indexOf_metric(abbreviation);
    return index == NOT_A_METRIC_ID ? DESCRIPTION_OF_NOT_A_METRIC : metric_descriptions[index];
}

void showListOf_metrics(void) {
    fprintf(stderr, "Supported Metrics =\n    %s", metrics[0]);
    for (char const** metric = metrics + 1; *metric; metric++) {
        fprintf(stderr, "\n    %s", *metric);
    }
    fputs("\n", stderr);
}

bool isValidIndex_metric(size_t const metricId) {
    return metricId < (sizeof(metrics) / sizeof(char const*));
}

bool enableOrExclude_metric(char const* const restrict abbreviation, bool const enable) {
    uint_fast64_t const metricId = indexOf_metric(abbreviation);
    unless (isValidIndex_metric(metricId)) return 0;
    if (enable) {
        if (options.enabledMetrics == ALL_METRICS_ENABLED) options.enabledMetrics = 0;
        options.enabledMetrics |= ((uint_fast64_t)1 << metricId);
    } else {
        options.enabledMetrics &= ~((uint_fast64_t)1 << metricId);
    }
    return 1;
}
