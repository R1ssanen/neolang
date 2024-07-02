#include "parser.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

static Parser* State = NULL;

void           InitParser(const Token* Tokens, u64 TokenCount) {
    if (!Tokens) { ARG_ERR("Null input tokens."); }

    State  = Alloc(Parser, 1);
    *State = (Parser){ .Tokens = Tokens, .TokenCount = TokenCount, .TokenIndex = 0 };
}

u64          GetIndex(void) { return State->TokenIndex; }

void         Rollback(u64 Offset) { State->TokenIndex -= Offset; }

const Token* Peek(u32 Offset) {
    // printf("%lu, %lu\n\n", State->TokenIndex, State->TokenCount);

    if ((State->TokenIndex + Offset) >= State->TokenCount) { return NULL; }
    return State->Tokens + (State->TokenIndex + Offset);
}

const Token* Consume(void) { return State->Tokens + (State->TokenIndex++); }

const Token* TryConsumeType(TokenType Type) {
    if (Peek(0) && Peek(0)->Type == Type) { return Consume(); }
    return NULL;
}

const Token* TryConsumeSub(TokenSubtype Sub) {
    if (Peek(0) && Peek(0)->Subtype == Sub) { return Consume(); }
    return NULL;
}
