/**
 * @file unit.h
 * @brief Defines Unit
 * @author Yavuz Koroglu
 */
#ifndef UNIT_H
    #define UNIT_H

    /**
     * @struct Unit
     * @brief In C, every file is a separate unit.
     *
     * @var Unit::name
     *   Name of the unit.
     */
    typedef struct UnitBody {
        char const* name;
    } Unit;
#endif
