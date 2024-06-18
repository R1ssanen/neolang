#ifndef PARSE_H
#define PARSE_H

#include "../lexer/token_types.h"
#include "../types.h"

struct NodeRoot;

b8 Parse(const Token* Tokens, u64 TokensLen, struct NodeRoot* Tree);

#endif
