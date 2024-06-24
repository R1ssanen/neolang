#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

static Tokenizer* State = NULL;

Error*            InitTokenizer(const char* Source, u64 SourceLen) {
    if (!Source) { return ERROR(_INVALID_ARG, "Null source input."); }

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

    if (WithinComment) { return ERROR(_SYNTAX_ERROR, "Unclosed comment block."); }

    // concatenate segment after last comment
    strncat(Buffer, Source + LastCommentEnd, SourceLen - LastCommentEnd);

    // split uncommented source by line breaks
    const u64   MAX_BITES = 10000;
    const char* DELIM     = "\n\v\f";
    State->Bites          = Alloc(char*, MAX_BITES);
    State->BitesLen       = 0;

    for (char* Bite = strtok(Buffer, DELIM); Bite; Bite = strtok(NULL, DELIM)) {
        State->Bites[State->BitesLen] = Alloc(char, strlen(Bite) + 1);
        strcpy(State->Bites[State->BitesLen], Bite);

        ++State->BitesLen;
    }

    State->CurrBite  = State->Bites[0];
    State->BiteIndex = 0;
    State->CharIndex = 0;

    return NO_ERROR;
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
