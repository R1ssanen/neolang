/**
 * @file tokenize.h
 * @brief Tokenizer
 * @author github.com/r1ssanen
 */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "token_types.h"
#include "types.h"

b8 Tokenize(const char* Source, u64 SourceLen, Token* Tokens, u64* TokensLen);

#endif
