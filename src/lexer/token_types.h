/**
 * @file lexer/token_types.h
 * @brief Token type enumerations.
 * @author github.com/r1ssanen
 */

#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

extern const char* SPECIAL_SYMBOLS;
extern const char* OPERATORS;
extern const char* KEYWORDS[];
extern const char* BUILTIN_TYPES[];

typedef enum TokenType {
    _INVALID = 0,

    // identifier
    _ID      = 0x00000001,

    // keyword
    _KEY     = 0x00000002,

    // operator
    _OP      = 0x00000004,

    // string literal
    _STRLIT  = 0x00000008,

    // numeric literal
    _NUMLIT  = 0x00000010,

    // special symbol
    _SPEC    = 0x00000020,

    // built-in type
    _BITYPE  = 0x00000040
} TokenType;

const char* GetTypeDebugName(TokenType Type);

#include "token_subtypes.h"

typedef struct Token {
    void*        Value;
    TokenType    Type;
    TokenSubtype Subtype;
} Token;

#endif
