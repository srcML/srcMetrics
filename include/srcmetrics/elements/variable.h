#ifndef VARIABLE_H
    #define VARIABLE_H
    #include <stdbool.h>

    typedef struct VariableBody {
        char const* name;
        char const* ownerFn;
        char const* ownerUnit;
        bool        is_scope_global;
    } Variable;
#endif
