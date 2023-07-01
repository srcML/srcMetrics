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

bool reportCsv(void) {
    static Report* reports[] = REPORTS;

    FILE* output = options.outfile ? fopen(options.outfile, "r") : stdout; {
        unless (output) return 0;
        {
            Report** report             = reports;
            uint_fast64_t metricMask    = 1;
            fprintf(output, "NAME%sVALUE%s", csv_delimeter, csv_row_end);
            for (; *report && metricMask; report++, metricMask <<= 1) {
                unless (options.enabledMetrics & metricMask) continue;
                (**report)(output);
            }
        }
    } if (options.outfile) fclose(output);

    return 1;
}
