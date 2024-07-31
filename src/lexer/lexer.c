#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../util/arena.h"
#include "../util/assert.h"

static Lexer* State = NULL;

void          InitLexer(const char* Source, u64 SourceLen) {
    NASSERT_MSG(Source, "Null source string.");

    State  = Alloc(Lexer, 1);
    *State = (Lexer){ .Source = Source, .SourceLen = SourceLen, .Line = 1, .Column = 1 };
}

char PeekChar(u64 Offset) {
    if ((State->Index + Offset) >= State->SourceLen) { return '\0'; }
    return State->Source[State->Index + Offset];
}

char ConsumeChar(void) {
    if (State->Source[State->Index] == '\n') {
        ++State->Line;
        State->Column = 1;
    } else {
        ++State->Column;
    }

    return State->Source[State->Index++];
}

char TryConsumeChar(char c) {
    if (PeekChar(0) != c) { return '\0'; }
    return ConsumeChar();
}

u64 GetLine(void) { return State->Line; }

u64 GetColumn(void) { return State->Column; }
