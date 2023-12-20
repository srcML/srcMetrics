/**
 * @file metrics.c
 * @brief Implements the functions defined in metrics.h
 * @author Yavuz Koroglu
 * @see metrics.h
 */
#include <stdint.h>
#include <string.h>

#include "srcmetrics.h"
#include "srcmetrics/metrics.h"
#include "padkit/chunk.h"
#include "padkit/streq.h"

static char const* metrics[]             = METRICS;
static char const* metric_descriptions[] = METRIC_DESCRIPTIONS;

size_t indexOf_metric(char const* const abbreviation) {
    if (abbreviation == NULL) return NOT_A_METRIC_ID;
    for (size_t metricId = 0; metrics[metricId]; metricId++) {
        if (str_eq(abbreviation, metrics[metricId])) return metricId;
    }
    return NOT_A_METRIC_ID;
}

char const* descriptionOf_metric(char const* const abbreviation) {
    size_t const index = indexOf_metric(abbreviation);
    if (index == NOT_A_METRIC_ID) return DESCRIPTION_OF_NOT_A_METRIC;
    return metric_descriptions[index];
}

void showListOf_metrics(void) {
    fputs("\n"
          "Supported Metrics =", stderr);
    for (char const** metric = metrics; *metric; metric++) {
        fprintf(stderr, "\n    %*s: %s", 5, *metric, descriptionOf_metric(*metric));
    }
    fputs("\n\n", stderr);
}

bool isValidIndex_metric(size_t const metricId) {
    return metricId < (sizeof(metrics) / sizeof(char const*));
}

bool enableOrExclude_metric(char const* const abbreviation, bool const enable) {
    size_t const metricId = indexOf_metric(abbreviation);
    if (!isValidIndex_metric(metricId)) return 0;
    if (enable) {
        if (options.enabledMetrics == ALL_METRICS_ENABLED) options.enabledMetrics = 0;
        options.enabledMetrics |= ((uint_fast64_t)1 << metricId);
    } else {
        options.enabledMetrics &= ~((uint_fast64_t)1 << metricId);
    }
    return 1;
}
