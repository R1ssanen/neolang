/**
 * @file lexer/lexer.h
 * @brief Lexer internal state.
 * @author github.com/r1ssanen
 */

#ifndef LEXER_H
#define LEXER_H

#include "../types.h"

typedef struct Lexer {
    const char* Source;
    u64         SourceLen;
    u64         Index;
    u64         Line;
    u64         Column;
} Lexer;

void InitLexer(const char* Source, u64 SourceLen);

char PeekChar(u64 Offset);
char ConsumeChar(void);
char TryConsumeChar(char c);

u64  GetLine(void);
u64  GetColumn(void);

#endif
