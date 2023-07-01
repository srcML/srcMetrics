/**
 * @file value.h
 * @brief Defines Value.
 * @author Yavuz Koroglu
 * @see SVPair
 */
#ifndef VALUE_H
    #define VALUE_H
    #include <inttypes.h>

    typedef char*       value_t_string;
    typedef void*       value_t_pointer;
    typedef double      value_t_double;
    typedef int64_t     value_t_signed;
    typedef uint64_t    value_t_unsigned;

    typedef union value_t_body {
        value_t_string      as_string;
        value_t_pointer     as_pointer;
        value_t_double      as_double;
        value_t_signed      as_signed;
        value_t_unsigned    as_unsigned;
    } value_t;

    #define VAL_TC_STRING   0
    #define VAL_TC_POINTER  1
    #define VAL_TC_DOUBLE   2
    #define VAL_TC_SIGNED   3
    #define VAL_TC_UNSIGNED 4

    #define VAL_F_STRING    "%s"
    #define VAL_F_POINTER   "%p"
    #define VAL_F_DOUBLE    "%.2lf"
    #define VAL_F_SIGNED    PRId64
    #define VAL_F_UNSIGNED  PRIu64

    #define VAL_STRING(str)     ((Value){ (value_t){ .as_string     = str },    VAL_TC_STRING   })
    #define VAL_POINTER(ptr)    ((Value){ (value_t){ .as_pointer    = ptr },    VAL_TC_POINTER  })
    #define VAL_DOUBLE(dbl)     ((Value){ (value_t){ .as_double     = dbl },    VAL_TC_DOUBLE   })
    #define VAL_SIGNED(num)     ((Value){ (value_t){ .as_signed     = num },    VAL_TC_SIGNED   })
    #define VAL_UNSIGNED(num)   ((Value){ (value_t){ .as_unsigned   = num },    VAL_TC_UNSIGNED })

    /**
     * @struct Value
     * @brief A Value could be one of several types.
     *
     * @var Value::value
     *   The value.
     * @var Value::type_code
     *   Type-code of the value.
     */
    typedef struct ValueBody {
        value_t     value;
        unsigned    type_code;
    } Value;
#endif
