#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

extern const char* SPECIAL_SYMBOLS;
extern const char* OPERATORS;
extern const char* KEYWORDS[];

typedef enum TokenType {
    _INVALID = 0,    // invalid token type
    _ID      = 0x1,  // identifier
    _KEY     = 0x2,  // keyword
    _OP      = 0x4,  // operator
    _STRLIT  = 0x8,  // string literal
    _NUMLIT  = 0x10, // numeric literal
    _SPEC    = 0x20, // special character
} TokenType;

const char* GetTypeDebugName(TokenType Type);

typedef struct Token {
    void*     Value;
    void*     Subtype;
    TokenType Type;
} Token;

#endif
