/**
 * @file parser/parser.h
 * @brief Parsers internal state.
 * @author github.com/r1ssanen
 */

#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/error.h"

typedef struct Parser {
    Token* Tokens;
    u64    TokensLen;
    u64    TokenIndex;
} Parser;

Error* InitParser(const Token* Tokens, u64 TokensLen);

Token* Peek(u32 Offset);

Token* Consume(void);

#endif
