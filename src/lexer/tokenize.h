/**
 * @file lexer/tokenize.h
 * @brief Tokenizer.
 * @author github.com/r1ssanen
 */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "types.h"

struct Token;
struct Token* Tokenize(const char* Source, u64* TokensLen);

#endif
