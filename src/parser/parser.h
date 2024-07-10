/**
 * @file parser/parser.h
 * @brief Parsers internal state.
 * @author github.com/r1ssanen
 */

#ifndef PARSER_H
#define PARSER_H

#include "../lexer/token_types.h"
#include "../types.h"

typedef struct Parser {
    Token* Tokens;
    u64    TokenCount;
    u64    TokenIndex;
} Parser;

void   InitParser(const Token* Tokens, u64 TokenCount);

Token* Peek(u32 Offset);
Token* Consume(void);
Token* TryConsumeType(TokenType Type);
Token* TryConsumeSub(TokenSubtype Sub);

#endif
