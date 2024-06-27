#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

static Parser* State = NULL;

b8             InitParser(const Token* Tokens, u64 TokensLen) {
    if (!Tokens) {
        THROW_ERROR(_INVALID_ARG, "Null input tokens.");
        return false;
    }

    State             = Alloc(Parser, 1);
    State->Tokens     = (Token*)Tokens;
    State->TokensLen  = TokensLen;
    State->TokenIndex = 0;

    return true;
}

Token* Peek(u32 Offset) {
    if ((State->TokenIndex + Offset) >= State->TokensLen) { return NULL; }
    return State->Tokens + (State->TokenIndex + Offset);
}

Token* Consume(void) { return State->Tokens + (State->TokenIndex++); }

Token* TryConsumeType(TokenType Type) {
    if (Peek(0) && Peek(0)->Type == Type) { return Consume(); }
    return NULL;
}

Token* TryConsumeSub(TokenSubtype Sub) {
    if (Peek(0) && Peek(0)->Subtype == Sub) { return Consume(); }
    return NULL;
}
