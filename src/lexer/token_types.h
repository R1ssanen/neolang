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
extern const char* BITYPES[];

typedef enum TokenType {
    _INVALID = 0,

    _ID,
    _KEY,
    _OP,
    _STRLIT,
    _NUMLIT,
    _SPEC,
    _BITYPE,
} TokenType;

const char* GetTypeDebugName(TokenType Type);

#include "../types.h"
#include "token_subtypes.h"

typedef struct Token {
    void*        Value;
    TokenType    Type;
    TokenSubtype Subtype;
    u64          Line;
    u64          Column;
} Token;

#endif
