#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "types.h"

typedef enum Token {
    _ID = 0, // identifier
    _KEY,    // keyword
    _OP,     // operator
    _STR,    // string literal
    _SPEC,   // special character
    _CONST,  // constant value

    __MAX
} Token;

const char* GetTokenDebugName(Token Token);

b8          Tokenize(const char* pSource, u64 SourceLen, Token* pOutTokens, u64* pOutTokensLen);

#endif
