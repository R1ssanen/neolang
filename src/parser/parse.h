/**
 * @file parser/parse.h
 * @brief Parse.
 * @author github.com/r1ssanen
 */

#ifndef PARSE_H
#define PARSE_H

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/error.h"

struct NodeRoot;

Error* Parse(const Token* Tokens, u64 TokensLen, struct NodeRoot* Tree);

#endif
