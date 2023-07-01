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
#include "util/unless.h"

bool reportCsv(void) {
    static Report   reports[]   = REPORTS;
    Report*         report      = NULL;
    SVMap*          statistics  = NULL;
    uint_fast64_t   metricMask  = 0;
    FILE*           output      = NULL;

    unless ((output = options.outfile ? fopen(options.outfile, "r") : stdout)) return 0;
    fprintf(output, "NAME%sVALUE%s", csv_delimeter, csv_row_end);
    for (report = reports, metricMask = 1; *report && metricMask; report++, metricMask <<= 1) {
        unless ((options.enabledMetrics & metricMask) && (statistics = (*report)())) continue;
        for (SVPair* pair = statistics->pairs; pair < statistics->pairs + statistics->size; pair++) {
            fputs(pair->key, output);
            fputs(csv_delimeter, output);
            switch (pair->value.type_code) {
                case VAL_TC_STRING:     fprintf(output, VAL_F_STRING, pair->value.raw.as_string);       break;
                case VAL_TC_POINTER:    fprintf(output, VAL_F_POINTER, pair->value.raw.as_pointer);     break;
                case VAL_TC_DOUBLE:     fprintf(output, VAL_F_DOUBLE, pair->value.raw.as_double);       break;
                case VAL_TC_SIGNED:     fprintf(output, VAL_F_SIGNED, pair->value.raw.as_signed);       break;
                case VAL_TC_UNSIGNED:   fprintf(output, VAL_F_UNSIGNED, pair->value.raw.as_unsigned);   break;
                default:                return 0;
            }
            fputs(csv_row_end, output);
        }
    }
    if (options.outfile) fclose(output);

    return 1;
}
