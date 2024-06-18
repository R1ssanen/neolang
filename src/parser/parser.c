#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token_types.h"
#include "../types.h"

static Parser* State = NULL;

b8             InitParser(const Token* Tokens, u64 TokensLen) {
    if (!Tokens) {
        fputs("ParseError: Null input tokens.\n", stderr);
        return false;
    }

    State         = malloc(sizeof(Parser));
    State->Tokens = malloc(TokensLen * sizeof(Token));

    if (!memcpy((void*)State->Tokens, (void*)Tokens, TokensLen * sizeof(Token))) {
        fputs("Error: memcpy failed at 'InitParser'.\n", stderr);
        return false;
    }

    State->TokensLen  = TokensLen;
    State->TokenIndex = 0;

    return true;
}

void   DestroyParser(void) { free((void*)State); }

Token* Peek(u32 Offset) {
    if ((State->TokenIndex + Offset) >= State->TokensLen) { return NULL; }

    return &State->Tokens[State->TokenIndex + Offset];
}

Token* Consume(void) {
    if (State->TokenIndex >= State->TokensLen) { return NULL; }

    ++State->TokenIndex;
    return &State->Tokens[State->TokenIndex];
}

b8 AtLeast(u32 Count) { return Count <= (State->TokensLen - State->TokenIndex); }
