/**
 * @file report.h
 * @brief Defines the reportCsv() function.
 * @author Yavuz Koroglu
 * @see report.c
 */
#ifndef REPORT_H
    #define REPORT_H
    #include "srcmetrics/metrics/abc.h"
    #include "srcmetrics/metrics/ams.h"
    #include "srcmetrics/metrics/cc.h"
    #include "srcmetrics/metrics/hsm.h"
    #include "srcmetrics/metrics/mc.h"
    #include "srcmetrics/metrics/mnd.h"
    #include "srcmetrics/metrics/npm.h"
    #include "srcmetrics/metrics/rfu.h"
    #include "srcmetrics/metrics/sloc.h"

    typedef Map const*(*Report)(void);

    /**
     * @brief Calls all enabled metric Report functions.
     */
    #ifndef NDEBUG
    bool
    #else
    void
    #endif
    reportCsv(void);

    #define REPORTS {   \
        ABC_REPORT,     \
        AMS_REPORT,     \
        CC_REPORT,      \
        HSM_REPORT,     \
        MC_REPORT,      \
        MND_REPORT,     \
        NPM_REPORT,     \
        RFU_REPORT,     \
        SLOC_REPORT,    \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL,           \
        NULL            \
    }
#endif
