#include "tokenizer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

static Tokenizer* State = NULL;

b8                InitTokenizer(const char* Source, u64 SourceLen) {
    if (!Source) {
        THROW_ERROR(_INVALID_ARG, "Null source input.");
        return false;
    }

    State                      = Alloc(Tokenizer, 1);

    // strip comments
    const char* Result         = Source;
    char*       Buffer         = Alloc(char, SourceLen);
    b8          WithinComment  = false;
    u64         LastCommentEnd = 0;

    while ((Result = strchr(Result, '$'))) {
        WithinComment = !WithinComment;

        if (!WithinComment) { // exited
            LastCommentEnd = (Result - Source) + 1;
        } else { // entered
            strncat(Buffer, Source + LastCommentEnd, (Result - Source) - LastCommentEnd);
        }

        ++Result;
    }

    if (WithinComment) {
        THROW_ERROR(_SYNTAX_ERROR, "Unclosed comment block.");
        return false;
    }

    // concatenate segment after last comment
    strncat(Buffer, Source + LastCommentEnd, SourceLen - LastCommentEnd);

    // split uncommented source by line breaks
    const u64   MAX_BITES = 10000;
    const char* DELIM     = "\n\v\f";
    State->Bites          = Alloc(char*, MAX_BITES);
    State->BitesLen       = 0;

    for (char* Bite = strtok(Buffer, DELIM); Bite; Bite = strtok(NULL, DELIM)) {

        b8 Empty = true;
        for (u64 i = 0; i < strlen(Bite); ++i) {
            if (!isspace(Bite[i])) { Empty = false; }
        }

        if (Empty) { continue; }

        State->Bites[State->BitesLen] = Alloc(char, strlen(Bite) + 1);
        strcpy(State->Bites[State->BitesLen], Bite);

        ++State->BitesLen;
    }

    State->CurrBite  = State->Bites[0];
    State->BiteIndex = 0;
    State->CharIndex = 0;

    return true;
}

char PeekChar(u64 Offset) {
    if ((State->CharIndex + Offset) >= strlen(State->CurrBite)) { return '\0'; }
    return State->CurrBite[State->CharIndex + Offset];
}

/**
 * @brief Return current character and advance.
 * @return Current character
 */
char  ConsumeChar(void) { return State->CurrBite[State->CharIndex++]; }

char* PeekBite(u64 Offset) {
    if ((State->BiteIndex + Offset) >= State->BitesLen) { return NULL; }
    return State->Bites[State->BiteIndex + Offset];
}

char* ConsumeBite(void) {
    State->CharIndex = 0;
    ++State->BiteIndex;
    State->CurrBite = State->Bites[State->BiteIndex];

    return State->CurrBite;
}
