/**
 * @file report.h
 * @brief Defines the reportCsv() function.
 * @author Yavuz Koroglu
 * @see report.c
 */
#ifndef REPORT_H
    #define REPORT_H
    #include "util/svmap.h"

    #include "srcmetrics/metrics/npm.h"
    #include "srcmetrics/metrics/sloc.h"

    typedef SVMap*(*Report)(void);

    /**
     * @brief Calls all enabled metric Report functions.
     * @return 0 if something is wrong report, 1 otherwise.
     */
    bool reportCsv(void);

    #define REPORTS {   \
        NPM_REPORT,     \
        SLOC_REPORT,    \
        NULL            \
    }
#endif
