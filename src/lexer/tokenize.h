/**
 * @file lexer/tokenize.h
 * @brief Tokenizer.
 * @author github.com/r1ssanen
 */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "../util/error.h"
#include "token_types.h"
#include "types.h"

Error* Tokenize(const char* Source, u64 SourceLen, Token* Tokens, u64* TokensLen);

#endif
