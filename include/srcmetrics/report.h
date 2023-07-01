/**
 * @file report.h
 * @brief Defines the Report function-type.
 * @author Yavuz Koroglu
 * @see report.c
 */
#ifndef REPORT_H
    #define REPORT_H
    #include <stdio.h>

    typedef void(*Report)(FILE* output);

    /**
     * @brief Calls all enabled metric Report functions.
     * @return 0 if there is something wrong with the output, 1 otherwise.
     */
    bool reportCsv(void);

    #define REPORTS {   \
        NPM_REPORT,     \
        SLOC_REPORT,    \
        NULL            \
    }
#endif
