/**
 * @file tokenize.h
 * @brief Tokenizer
 * @author github.com/r1ssanen
 */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "token_types.h"
#include "types.h"

/**
 * @brief Tokenize a source string.
 *
 * @param pSource Source string to tokenize
 * @param SourceLen Length of source string
 * @param pTokens Output tokens
 * @param pTokensLen Length of output tokens
 * @return False on failure
 */
b8 Tokenize(const char* pSource, u64 SourceLen, Token* pTokens, u64* pTokensLen);

/**
 * @brief Try getting a token from string.
 *
 * @param Str String to tokenize
 * @return
 */
Token TryGetToken(const char* Str);

#endif
