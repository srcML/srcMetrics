/**
 * @file function.h
 * @brief Defines Function.
 * @author Yavuz Koroglu
 * @see Function
 */
#ifndef FUNCTION_H
    #define FUNCTION_H

    #define FN_COUNT_GUESS 16384

    /**
     * @struct Function
     * @brief A Function is an essential construct in almost every procedural language.
     *
     * @var Function::name
     *   Name of the function.
     * @var Function::ownerUnit
     *   Name of the owner unit.
     * @var Function::designator
     *   The function designator, a.k.a "owner::function.name".
     *
     * @see Unit
     */
    typedef struct FunctionBody {
        char const* name;
        char const* ownerUnit;
        char const* designator;
    } Function;
#endif
