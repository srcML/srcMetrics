/**
 * @file report.c
 * @brief Implements functions defined in report.h
 * @author Yavuz Koroglu
 * @see report.h
 */
#include "srcmetrics.h"
#include "srcmetrics/metrics.h"
#include "srcmetrics/report.h"
#include "padkit/chunk.h"
#include "padkit/csv.h"
#include "padkit/map.h"

#ifndef NDEBUG
bool
#else
void
#endif
reportCsv(void) {
    static Report reports[]      = REPORTS;
    Report*       report         = NULL;
    Map const*    statistics     = NULL;
    uint_fast64_t enabledMetrics = options.enabledMetrics;
    FILE* const   output         = options.outfile ? fopen(options.outfile, "w") : stdout;

    #ifndef NDEBUG
        if (output == NULL) return 0;
    #endif
    for (report = reports; enabledMetrics; report++, enabledMetrics >>= 1) {
        if (report == NULL || *report == NULL || !(enabledMetrics & 1)) continue;
        statistics = (*report)();
        if (statistics == NULL && !isValid_map(statistics)) continue;
        for (Mapping* mapping = statistics->mappings; mapping < statistics->mappings + statistics->size; mapping++) {
            char const* key = get_chunk(strings, mapping->key_id);
            #ifndef NDEBUG
                if (key == NULL)                         return 0;
                if (fputs(key, output) == EOF)           return 0;
                if (fputs(csv_delimeter, output) == EOF) return 0;
            #else
                fputs(key, output);
                fputs(csv_delimeter, output);
            #endif
            switch (mapping->value.type_code) {
                case VAL_TC_FLOAT:
                    #ifndef NDEBUG
                        if (fprintf(output, VAL_F_FLOAT, (double)mapping->value.raw.as_float) < 0) return 0;
                    #else
                        fprintf(output, VAL_F_FLOAT, (double)mapping->value.raw.as_float);
                    #endif
                    break;
                case VAL_TC_INT:
                    #ifndef NDEBUG
                        if (fprintf(output, VAL_F_INT, mapping->value.raw.as_int) < 0) return 0;
                    #else
                        fprintf(output, VAL_F_INT, mapping->value.raw.as_int);
                    #endif
                    break;
                case VAL_TC_UNSIGNED:
                    #ifndef NDEBUG
                        if (fprintf(output, VAL_F_UNSIGNED, mapping->value.raw.as_unsigned) < 0) return 0;
                    #else
                        fprintf(output, VAL_F_UNSIGNED, mapping->value.raw.as_unsigned);
                    #endif
                    break;
                default:
                    #ifndef NDEBUG
                        if (fputs("NOT_A_VALUE", output) == EOF) return 0;
                    #else
                        fputs("NOT_A_VALUE", output);
                    #endif
            }
            #ifndef NDEBUG
                if (fputs(csv_row_end, output) == EOF) return 0;
            #else
                fputs(csv_row_end, output);
            #endif
        }
    }
    #ifndef NDEBUG
        if (options.outfile && fclose(output) == EOF) return 0;
        return 1;
    #else
        if (options.outfile) fclose(output);
    #endif
}
