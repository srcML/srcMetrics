#ifndef STREQ_H
    #define STREQ_H
    #include <string.h>

    #define str_eq_const(str,conststr)   (!strncmp(str,conststr,sizeof(conststr)-1))
#endif
