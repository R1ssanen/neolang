#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

#include "types.h"

typedef enum TokenType {
    _INVALID = 0,    // invalid token type
    _ID      = 0x1,  // identifier
    _KEY     = 0x2,  // keyword
    _OP      = 0x4,  // operator
    _STRLIT  = 0x8,  // string literal
    _NUMLIT  = 0x10, // numeric literal
    _SPEC    = 0x20, // special character
} TokenType;

TokenType   Classify(const char* Str);
const char* GetTypeDebugName(TokenType Type);

typedef struct Token {
    void*     Value;
    void*     SubType;
    TokenType BroadType;
    u64       Line;
    u64       Character;
} Token;

Token TryGetKeyword(const char* Str);
Token TryGetOperator(const char* Str);
Token TryGetSpecial(const char* Str);
Token TryGetNumericLiteral(const char* Str);
Token TryGetStringLiteral(const char* Str);
Token TryGetIdentifier(const char* Str);

#endif
