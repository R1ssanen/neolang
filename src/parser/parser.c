#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

static Parser* State = NULL;

Error*         InitParser(const Token* Tokens, u64 TokensLen) {
    if (!Tokens) { return ERROR(_INVALID_ARG, "Null input tokens."); }

    State             = Alloc(Parser, 1);
    State->Tokens     = Tokens;
    State->TokensLen  = TokensLen;
    State->TokenIndex = 0;

    return NO_ERROR;
}

Token* Peek(u32 Offset) {
    if ((State->TokenIndex + Offset) >= State->TokensLen) { return NULL; }
    return State->Tokens + (State->TokenIndex + Offset);
}

Token* Consume(void) {
    ++State->TokenIndex;
    return State->Tokens + (State->TokenIndex);
}
