#ifndef STREQ_H
    #define STREQ_H
    #include <string.h>

    #define str_eq_const(str,strconst)   (!strncmp(str,strconst,sizeof(strconst)-1))
#endif
