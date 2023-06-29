/**
 * @file metrics.c
 * @brief Implements the functions defined in metrics.h
 * @author Yavuz Koroglu
 * @see metrics.h
 */
#include <stdio.h>

#include "srcmetrics/metrics.h"
#include "util/streq.h"
#include "util/unless.h"

static char const* metrics[] = METRICS;
static char const* metric_descriptions[] = METRIC_DESCRIPTIONS;

ssize_t indexOf_metric(char const* const restrict abbreviation) {
    for (char const** metric = metrics; *metric; metric++) {
        unless (str_eq_const(abbreviation, *metric)) continue;
        return metric - metrics;
    }
    return NOT_A_METRIC;
}

char const* descriptionOf_metric(char const* const restrict abbreviation) {
    ssize_t const index = indexOf_metric(abbreviation);
    return index < 0 ? DESCRIPTION_OF_NOT_A_METRIC : metric_descriptions[index];
}

void showListOf_metrics(void) {
    fprintf(stderr, "Supported Metrics =\n    %s", metrics[0]);
    for (char const** metric = metrics + 1; *metric; metric++) {
        fprintf(stderr, "\n    %s", *metric);
    }
    fputs("\n", stderr);
}
