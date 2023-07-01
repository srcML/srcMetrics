/**
 * @file report.c
 * @brief Implements functions defined in report.h
 * @author Yavuz Koroglu
 * @see report.h
 */
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
        for (SVPair* pair = statistics.pairs; pair < statistics.pairs + statistics.size; pair++) {
            switch (pair->value.type_code) {
                case VAL_TC_STRING:
                    fprintf(output, "%s%s"VAL_F_STRING"%s", pair->key, csv_delimeter, pair->value.as_string, csv_row_end);
                    break;
                case VAL_TC_POINTER:
                    fprintf(output, "%s%s"VAL_F_POINTER"%s", pair->key, csv_delimeter, pair->value.as_pointer, csv_row_end);
                    break;
                case VAL_TC_DOUBLE:
                    fprintf(output, "%s%s"VAL_F_DOUBLE"%s", pair->key, csv_delimeter, pair->value.as_double, csv_row_end);
                    break;
                case VAL_TC_SIGNED:
                    fprintf(output, "%s%s"VAL_F_SIGNED"%s", pair->key, csv_delimeter, pair->value.as_signed, csv_row_end);
                    break;
                case VAL_TC_UNSIGNED:
                    fprintf(output, "%s%s"VAL_F_UNSIGNED"%s", pair->key, csv_delimeter, pair->value.as_unsigned, csv_row_end);
                    break;
                default:
                    return 0;
            }
        }
    }
    if (options.outfile) fclose(output);

    return 1;
}
