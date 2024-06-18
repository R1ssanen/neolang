#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token_types.h"
#include "../types.h"

typedef struct Parser {
    Token* Tokens;
    u64    TokensLen;
    u64    TokenIndex;
} Parser;

b8     InitParser(const Token* Tokens, u64 TokensLen);

void   DestroyParser(void);

Token* Peek(u32 Offset);

Token* Consume(void);

b8     AtLeast(u32 Count);

#endif
