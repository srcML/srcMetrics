/**
 * @file event.h
 * @brief Defines the Event function.
 * @author Yavuz Koroglu
 * @see event.c
 */
#ifndef EVENT_H
    #define EVENT_H
    #include "srcmetrics/metrics/abc.h"
    #include "srcmetrics/metrics/ams.h"
    #include "srcmetrics/metrics/cc.h"
    #include "srcmetrics/metrics/hsm.h"
    #include "srcmetrics/metrics/mc.h"
    #include "srcmetrics/metrics/mnd.h"
    #include "srcmetrics/metrics/npm.h"
    #include "srcmetrics/metrics/rfu.h"
    #include "srcmetrics/metrics/sloc.h"

    typedef void(*Event)(struct srcsax_context*, ...);

    /**
     * @brief Gets a pointer to the static event handler for metrics.
     * @return A pointer to the metric event handler.
     */
    struct srcsax_handler* getStaticEventHandler(void);

    #define ALL_EVENTS_AT_START_DOCUMENT {  \
        ABC_EVENT_AT_START_DOCUMENT,        \
        AMS_EVENT_AT_START_DOCUMENT,        \
        CC_EVENT_AT_START_DOCUMENT,         \
        HSM_EVENT_AT_START_DOCUMENT,        \
        MC_EVENT_AT_START_DOCUMENT,         \
        MND_EVENT_AT_START_DOCUMENT,        \
        NPM_EVENT_AT_START_DOCUMENT,        \
        RFU_EVENT_AT_START_DOCUMENT,        \
        SLOC_EVENT_AT_START_DOCUMENT,       \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_DOCUMENT {    \
        ABC_EVENT_AT_END_DOCUMENT,          \
        AMS_EVENT_AT_END_DOCUMENT,          \
        CC_EVENT_AT_END_DOCUMENT,           \
        HSM_EVENT_AT_END_DOCUMENT,          \
        MC_EVENT_AT_END_DOCUMENT,           \
        MND_EVENT_AT_END_DOCUMENT,          \
        NPM_EVENT_AT_END_DOCUMENT,          \
        RFU_EVENT_AT_END_DOCUMENT,          \
        SLOC_EVENT_AT_END_DOCUMENT,         \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_ROOT {      \
        ABC_EVENT_AT_START_ROOT,            \
        AMS_EVENT_AT_START_ROOT,            \
        CC_EVENT_AT_START_ROOT,             \
        HSM_EVENT_AT_START_ROOT,            \
        MC_EVENT_AT_START_ROOT,             \
        MND_EVENT_AT_START_ROOT,            \
        NPM_EVENT_AT_START_ROOT,            \
        RFU_EVENT_AT_START_ROOT,            \
        SLOC_EVENT_AT_START_ROOT,           \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_UNIT {      \
        ABC_EVENT_AT_START_UNIT,            \
        AMS_EVENT_AT_START_UNIT,            \
        CC_EVENT_AT_START_UNIT,             \
        HSM_EVENT_AT_START_UNIT,            \
        MC_EVENT_AT_START_UNIT,             \
        MND_EVENT_AT_START_UNIT,            \
        NPM_EVENT_AT_START_UNIT,            \
        RFU_EVENT_AT_START_UNIT,            \
        SLOC_EVENT_AT_START_UNIT,           \
        NULL                                \
    }
    #define ALL_EVENTS_AT_START_ELEMENT {   \
        ABC_EVENT_AT_START_ELEMENT,         \
        AMS_EVENT_AT_START_ELEMENT,         \
        CC_EVENT_AT_START_ELEMENT,          \
        HSM_EVENT_AT_START_ELEMENT,         \
        MC_EVENT_AT_START_ELEMENT,          \
        MND_EVENT_AT_START_ELEMENT,         \
        NPM_EVENT_AT_START_ELEMENT,         \
        RFU_EVENT_AT_START_ELEMENT,         \
        SLOC_EVENT_AT_START_ELEMENT,        \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_ROOT {        \
        ABC_EVENT_AT_END_ROOT,              \
        AMS_EVENT_AT_END_ROOT,              \
        CC_EVENT_AT_END_ROOT,               \
        HSM_EVENT_AT_END_ROOT,              \
        MC_EVENT_AT_END_ROOT,               \
        MND_EVENT_AT_END_ROOT,              \
        NPM_EVENT_AT_END_ROOT,              \
        RFU_EVENT_AT_END_ROOT,              \
        SLOC_EVENT_AT_END_ROOT,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_UNIT {        \
        ABC_EVENT_AT_END_UNIT,              \
        AMS_EVENT_AT_END_UNIT,              \
        CC_EVENT_AT_END_UNIT,               \
        HSM_EVENT_AT_END_UNIT,              \
        MC_EVENT_AT_END_UNIT,               \
        MND_EVENT_AT_END_UNIT,              \
        NPM_EVENT_AT_END_UNIT,              \
        RFU_EVENT_AT_END_UNIT,              \
        SLOC_EVENT_AT_END_UNIT,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_END_ELEMENT {     \
        ABC_EVENT_AT_END_ELEMENT,           \
        AMS_EVENT_AT_END_ELEMENT,           \
        CC_EVENT_AT_END_ELEMENT,            \
        HSM_EVENT_AT_END_ELEMENT,           \
        MC_EVENT_AT_END_ELEMENT,            \
        MND_EVENT_AT_END_ELEMENT,           \
        NPM_EVENT_AT_END_ELEMENT,           \
        RFU_EVENT_AT_END_ELEMENT,           \
        SLOC_EVENT_AT_END_ELEMENT,          \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CHARACTERS_ROOT { \
        ABC_EVENT_AT_CHARACTERS_ROOT,       \
        AMS_EVENT_AT_CHARACTERS_ROOT,       \
        CC_EVENT_AT_CHARACTERS_ROOT,        \
        HSM_EVENT_AT_CHARACTERS_ROOT,       \
        MC_EVENT_AT_CHARACTERS_ROOT,        \
        MND_EVENT_AT_CHARACTERS_ROOT,       \
        NPM_EVENT_AT_CHARACTERS_ROOT,       \
        RFU_EVENT_AT_CHARACTERS_ROOT,       \
        SLOC_EVENT_AT_CHARACTERS_ROOT,      \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CHARACTERS_UNIT { \
        ABC_EVENT_AT_CHARACTERS_UNIT,       \
        AMS_EVENT_AT_CHARACTERS_UNIT,       \
        CC_EVENT_AT_CHARACTERS_UNIT,        \
        HSM_EVENT_AT_CHARACTERS_UNIT,       \
        MC_EVENT_AT_CHARACTERS_UNIT,        \
        MND_EVENT_AT_CHARACTERS_UNIT,       \
        NPM_EVENT_AT_CHARACTERS_UNIT,       \
        RFU_EVENT_AT_CHARACTERS_UNIT,       \
        SLOC_EVENT_AT_CHARACTERS_UNIT,      \
        NULL                                \
    }
    #define ALL_EVENTS_AT_META_TAG {        \
        ABC_EVENT_AT_META_TAG,              \
        AMS_EVENT_AT_META_TAG,              \
        CC_EVENT_AT_META_TAG,               \
        HSM_EVENT_AT_META_TAG,              \
        MC_EVENT_AT_META_TAG,               \
        MND_EVENT_AT_META_TAG,              \
        NPM_EVENT_AT_META_TAG,              \
        RFU_EVENT_AT_META_TAG,              \
        SLOC_EVENT_AT_META_TAG,             \
        NULL                                \
    }
    #define ALL_EVENTS_AT_COMMENT {         \
        ABC_EVENT_AT_COMMENT,               \
        AMS_EVENT_AT_COMMENT,               \
        CC_EVENT_AT_COMMENT,                \
        HSM_EVENT_AT_COMMENT,               \
        MC_EVENT_AT_COMMENT,                \
        MND_EVENT_AT_COMMENT,               \
        NPM_EVENT_AT_COMMENT,               \
        RFU_EVENT_AT_COMMENT,               \
        SLOC_EVENT_AT_COMMENT,              \
        NULL                                \
    }
    #define ALL_EVENTS_AT_CDATA_BLOCK {     \
        ABC_EVENT_AT_CDATA_BLOCK,           \
        AMS_EVENT_AT_CDATA_BLOCK,           \
        CC_EVENT_AT_CDATA_BLOCK,            \
        HSM_EVENT_AT_CDATA_BLOCK,           \
        MC_EVENT_AT_CDATA_BLOCK,            \
        MND_EVENT_AT_CDATA_BLOCK,           \
        NPM_EVENT_AT_CDATA_BLOCK,           \
        RFU_EVENT_AT_CDATA_BLOCK,           \
        SLOC_EVENT_AT_CDATA_BLOCK,          \
        NULL                                \
    }
    #define ALL_EVENTS_AT_PROC_INFO {       \
        ABC_EVENT_AT_PROC_INFO,             \
        AMS_EVENT_AT_PROC_INFO,             \
        CC_EVENT_AT_PROC_INFO,              \
        HSM_EVENT_AT_PROC_INFO,             \
        MC_EVENT_AT_PROC_INFO,              \
        MND_EVENT_AT_PROC_INFO,             \
        NPM_EVENT_AT_PROC_INFO,             \
        RFU_EVENT_AT_PROC_INFO,             \
        SLOC_EVENT_AT_PROC_INFO,            \
        NULL                                \
    }
#endif

