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
    const Token* Tokens;
    u64          TokenCount;
    u64          TokenIndex;
} Parser;

void InitParser(const Token* Tokens, u64 TokenCount);

u64  GetIndex(void);
void Rollback(u64 Offset)

    const Token* Peek(u32 Offset);
const Token* Consume(void);

const Token* TryConsumeType(TokenType Type);
const Token* TryConsumeSub(TokenSubtype Sub);

#endif
