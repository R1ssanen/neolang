#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/assert.h"

static Parser* State = NULL;

void           InitParser(const Token* Tokens, u64 TokenCount) {
    NASSERT_MSG(Tokens, "Null input tokens.");

    State = Alloc(Parser, 1);
    *State = (Parser){ .Tokens = (Token*)Tokens, .TokenCount = TokenCount, .TokenIndex = 0 };
}

Token* Peek(u32 Offset) {
    if ((State->TokenIndex + Offset) >= State->TokenCount) { return NULL; }
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
