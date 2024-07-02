/**
 * @file lexer/opinfo.h
 * @brief Operator information.
 * @author github.com/r1ssanen
 */

#ifndef OPINFO_H
#define OPINFO_H

#include "../types.h"

typedef enum { _ASSOC_LEFT, _ASSOC_RIGHT } Associativity;

typedef struct OpInfo {
    Associativity Assoc;
    u8            Prec;
} OpInfo;

extern const OpInfo OPINFOS[];

#endif
