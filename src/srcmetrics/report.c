/**
 * @file report.c
 * @brief Implements functions defined in report.h
 * @author Yavuz Koroglu
 * @see report.h
 */
#include <stdint.h>
#include "srcmetrics/metrics.h"
#include "srcmetrics/options.h"
#include "srcmetrics/report.h"
#include "util/csv.h"
#include "util/svmap.h"

bool reportCsv(void) {
    static Report* reports[] = REPORTS;
    Report** report;
    SVPair* statistics;
    uint_fast64_t metricMask;
    FILE* output;

    unless ((output = options.outfile ? fopen(options.outfile, "r") : stdout)) return 0;
    fprintf(output, "NAME%sVALUE%s", csv_delimeter, csv_row_end);
    for (report = reports, metricMask = 1; *report && metricMask; report++, metricMask <<= 1) {
        unless ((options.enabledMetrics & metricMask) && (statistics = (**report)(output))) continue;
        for (SVPair* pair = statistics.pairs; pair < statistics.pairs + statistics.size; pair++)
            fprintf(output, "%s%s%.2lf%s", pair->key, csv_delimeter, pair->value.as_double, csv_row_end);
    }
    if (options.outfile) fclose(output);

    return 1;
}
