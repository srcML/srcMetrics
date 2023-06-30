/**
 * @file metrics.h
 * @brief Defines all metrics. It supports up to 64 metrics.
 * @author Yavuz Koroglu
 */
#ifndef METRICS_H
    #define METRICS_H
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>
    #include "srcmetrics/metrics/npm.h"
    #include "srcmetrics/metrics/sloc.h"

    #define NOT_A_METRIC_ID             ALL_METRICS_ENABLED
    #define DESCRIPTION_OF_NOT_A_METRIC "Unknown Metric"

    #define METRICS {                   \
        "NPM",                          \
        "SLOC",                         \
        NULL                            \
    }
    #define METRIC_DESCRIPTIONS {       \
        "Number of Public Methods",     \
        "Source Lines of Code",         \
        NULL                            \
    }

    #define EVENTS_AT_START_DOCUMENT {  \
        NPM_EVENT_AT_START_DOCUMENT,    \
        SLOC_EVENT_AT_START_DOCUMENT,   \
        NULL                            \
    }
    #define EVENTS_AT_END_DOCUMENT {    \
        NPM_EVENT_AT_END_DOCUMENT,      \
        SLOC_EVENT_AT_END_DOCUMENT,     \
        NULL                            \
    }
    #define EVENTS_AT_START_ROOT {      \
        NPM_EVENT_AT_START_ROOT,        \
        SLOC_EVENT_AT_START_ROOT,       \
        NULL                            \
    }
    #define EVENTS_AT_START_UNIT {      \
        NPM_EVENT_AT_START_UNIT,        \
        SLOC_EVENT_AT_START_UNIT,       \
        NULL                            \
    }
    #define EVENTS_AT_START_ELEMENT {   \
        NPM_EVENT_AT_START_ELEMENT,     \
        SLOC_EVENT_AT_START_ELEMENT,    \
        NULL                            \
    }
    #define EVENTS_AT_END_ROOT {        \
        NPM_EVENT_AT_END_ROOT,          \
        SLOC_EVENT_AT_END_ROOT,         \
        NULL                            \
    }
    #define EVENTS_AT_END_UNIT {        \
        NPM_EVENT_AT_END_UNIT,          \
        SLOC_EVENT_AT_END_UNIT,         \
        NULL                            \
    }
    #define EVENTS_AT_END_ELEMENT {     \
        NPM_EVENT_AT_END_ELEMENT,       \
        SLOC_EVENT_AT_END_ELEMENT,      \
        NULL                            \
    }
    #define EVENTS_AT_CHARACTERS_ROOT { \
        NPM_EVENT_AT_CHARACTERS_ROOT,   \
        SLOC_EVENT_AT_CHARACTERS_ROOT,  \
        NULL                            \
    }
    #define EVENTS_AT_CHARACTERS_UNIT { \
        NPM_EVENT_AT_CHARACTERS_UNIT,   \
        SLOC_EVENT_AT_CHARACTERS_UNIT,  \
        NULL                            \
    }
    #define EVENTS_AT_META_TAG {        \
        NPM_EVENT_AT_META_TAG,          \
        SLOC_EVENT_AT_META_TAG,         \
        NULL                            \
    }
    #define EVENTS_AT_COMMENT {         \
        NPM_EVENT_AT_COMMENT,           \
        SLOC_EVENT_AT_COMMENT,          \
        NULL                            \
    }
    #define EVENTS_AT_CDATA_BLOCK {     \
        NPM_EVENT_AT_CDATA_BLOCK,       \
        SLOC_EVENT_AT_CDATA_BLOCK,      \
        NULL                            \
    }
    #define EVENTS_AT_PROC_INFO {       \
        NPM_EVENT_AT_PROC_INFO,         \
        SLOC_EVENT_AT_PROC_INFO,        \
        NULL                            \
    }

    size_t indexOf_metric(char const* const restrict abbreviation);
    char const* descriptionOf_metric(char const* const restrict abbreviation);
    void showListOf_metrics(void);
    bool isValidIndex_metric(size_t const metricId);
    bool enableOrExclude_metric(char const* const restrict abbreviation, bool const enable);

    void registerEnabledMetricEvents(void);
    void collectMetrics(void);

    void event_startDocument(struct srcsax_context* context);
    void event_endDocument(struct srcsax_context* context);
    void event_startRoot(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri,
        int                             num_namespaces,
        struct srcsax_namespace const*  namespaces,
        int                             num_attributes,
        struct srcsax_attribute const*  attributes
    );
    void event_startUnit(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri,
        int                             num_namespaces,
        struct srcsax_namespace const*  namespaces,
        int                             num_attribute,
        struct srcsax_attribute const*  attributes
    );
    void event_startElement(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri,
        int                             num_namespaces,
        struct srcsax_namespace const*  namespaces,
        int                             num_attributes,
        struct srcsax_attribute const*  attributes
    );
    void event_endRoot(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri
    );
    void event_endUnit(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri
    );
    void event_endElement(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri
    );
    void event_charactersRoot(struct srcsax_context* context, char const* ch, int len);
    void event_charactersUnit(struct srcsax_context* context, char const* ch, int len);
    void event_metaTag(
        struct srcsax_context*          context,
        char const*                     localname,
        char const*                     prefix,
        char const*                     uri,
        int                             num_namespaces,
        struct srcsax_namespace const*  namespaces,
        int                             num_attributes,
        struct srcsax_attribute const*  attributes
    );
    void event_comment(struct srcsax_context* context, char const* value);
    void event_cdataBlock(struct srcsax_context * context, char const* value, int len);
    void event_procInfo(struct srcsax_context* context, char const* target, char const* data);
#endif
