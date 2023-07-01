/**
 * @file variable.h
 * @brief Defines Variable.
 */
#ifndef VARIABLE_H
    #define VARIABLE_H
    #include <stdbool.h>

    /**
     * @struct Variable
     * @brief Every language has variables.
     *
     * @var Variable::name
     *   The variable name.
     * @var Variable::ownerFn
     *   The owner function name, if such a function exists.
     * @var Variable::ownerUnit
     *   The owner unit name.
     * @var Variable::isScopeGlobal
     *   1 if the Variable is of global scope, 0 otherwise.
     */
    typedef struct VariableBody {
        char const* name;
        char const* ownerFn;
        char const* ownerUnit;
        bool        isScopeGlobal;
    } Variable;
#endif
