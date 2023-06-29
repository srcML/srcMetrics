/**
 * @file npm.c
 */
#include <stdarg.h>
#include "npm.h"

#define NPM_NOT_A_METHOD    0
#define NPM_ASSUMED_PUBLIC  1
#define NPM_FINAL_PRIVATE   2
#define NPM_FINAL_PUBLIC    3
#define NPM_READ_SPECIFIER  4

static unsigned npm = 0U;
static unsigned npmState = NPM_NOT_A_METHOD;

void startElement_npm(struct srcsax_context* context, ...) {
    char const* localname;
    unless (context) return;
    {
        va_list args;
        va_start(args, context);
        localname = va_arg(args, char const*);
        va_end(args);
    }
    switch (npmState) {
        case NPM_NOT_A_METHOD:
            if (str_eq_const(localname, "function")) { npmState = NPM_ASSUMED_PUBLIC; npm++; }
            break;
        case NPM_ASSUMED_PUBLIC:
            if (str_eq_const(localname, "specifier")) npmState = NPM_READ_SPECIFIER;
    }
}
void endElement(struct srcsax_context* context, ...) {
    char const* localname;
    unless (context) return;
    {
        va_list args;
        va_start(args, context);
        localname = va_arg(args, char const*);
        va_end(args);
    }
    if (str_eq_const(localname, "function")) npmState = NPM_NOT_A_METHOD;
    else if (npmState == NPM_READ_SPECIFIER && str_eq_const(localname, "specifier")) npmState = NPM_FINAL_PUBLIC;
}
void charactersUnit(struct srcsax_context* context, ...) {
    char const* ch;
    unless (context) return;
    {
        va_list args;
        va_start(args, context);
        ch = va_arg(args, char const*);
        va_end(args);
    }
    unless (npmState == NPM_READ_SPECIFIER && str_eq_const(ch, "static")) return;
    npmState = NPM_FINAL_PRIVATE;
    npm--;
}
