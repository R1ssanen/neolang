#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arena.h"
#include "../lexer/token_types.h"
#include "../types.h"

static Parser* State = NULL;

b8             InitParser(const Token* Tokens, u64 TokensLen) {
    if (!Tokens) {
        fputs("ParseError: Null input tokens.\n", stderr);
        return false;
    }

    State         = Alloc(Parser, 1);
    State->Tokens = Alloc(Token, TokensLen);

    if (!memcpy((void*)State->Tokens, (const void*)Tokens, TokensLen * sizeof(Token))) {
        fputs("Error: memcpy failed at 'InitParser'.\n", stderr);
        return false;
    }

    State->TokensLen  = TokensLen;
    State->TokenIndex = 0;

    return true;
}

void   DestroyParser(void) { State = NULL; }

Token* Peek(u32 Offset) {
    if ((State->TokenIndex + Offset) >= State->TokensLen) { return NULL; }
    return State->Tokens + (State->TokenIndex + Offset);
}

Token* Consume(void) {
    ++State->TokenIndex;
    return State->Tokens + (State->TokenIndex);
}
