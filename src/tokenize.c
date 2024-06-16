#include "tokenize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token_types.h"
#include "types.h"
#include "util/array.h"

b8 Tokenize(const char* Source, u64 SourceLen, Token* pTokens, u64* pTokensLen) {
    if (!Source) {
        fputs("TokenError: Tokenization failed, no sources provided.\n", stderr);
        return false;
    }

    if (!pTokens || !pTokensLen) {
        fputs("TokenError: Tokenization failed, null output parameter.\n", stderr);
        return false;
    }

    // remove comments
    const char* Result         = Source;
    char*       Buffer         = calloc(SourceLen, sizeof(char));
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

    // unclosed comment block
    if (WithinComment) {
        fputs("TokenError: Unclosed comment block.\n", stderr);
        return false;
    }

    // concatenate segment after last comment
    strncat(Buffer, Source + LastCommentEnd, SourceLen - LastCommentEnd);

    // split uncommented source by whitespace
    // into smaller bites
    char** Bites = CreateArray(char*);
    for (char* Bite = strtok(Buffer, " \t\r\n\v\f"); Bite; Bite = strtok(NULL, " \t\r\n\v\f")) {
        AppendArray(Bites, Bite);
    }

    u64 TotalBites = GetLengthArray(Bites);
    for (u64 Curr = 0; Curr < TotalBites; ++Curr) {
        u32  BiteLength = strlen(Bites[Curr]);

        char Bite[BiteLength];
        strcpy(Bite, Bites[Curr]);

        // bite is already a token
        Token PreToken = TryGetToken(Bite);
        if (PreToken.Type != _INVALID) {
            pTokens[(*pTokensLen)++] = PreToken;
            continue;
        }

        const char* Result        = Bite;
        u64         LastSpecIndex = 0;

        // find delimiting special characters
        while ((Result = strpbrk(Result, "[](){},:;."))) {
            u64 SpecIndex   = Result - Bite;
            u64 LeftsideLen = SpecIndex - LastSpecIndex;

            if (LeftsideLen == 0) {
                ++Result;
                continue;
            }

            char* LHS = calloc(LeftsideLen, sizeof(char));
            strncpy(LHS, Bite + LastSpecIndex, LeftsideLen);

            char Spec[2]             = { Bite[SpecIndex], '\0' };
            pTokens[(*pTokensLen)++] = TryGetToken(LHS);
            pTokens[(*pTokensLen)++] = TryGetToken(Spec);

            free(LHS);
            ++Result;
            LastSpecIndex = SpecIndex + 1;
        }

        // no special characters found
        if (LastSpecIndex == 0) {
            for (u64 i = 0; i < BiteLength; ++i) {
                char  Str[2] = { Bite[i], '\0' };
                Token Token  = TryGetToken(Str);

                if (Token.Type != _INVALID) { pTokens[(*pTokensLen)++] = Token; }
            }
        }
    }

    DestroyArray(Bites);
    return true;
}

// order of operations matters,
// keywords must be tested before identifiers,
// numeric literal before operator
static Token (*PFNTokenGetters[6])(const char*) = { TryGetKeyword,        TryGetSpecial,
                                                    TryGetNumericLiteral, TryGetOperator,
                                                    TryGetStringLiteral,  TryGetIdentifier };

Token TryGetToken(const char* Str) {
    Token Token = { 0 };

    // find first token type that is valid
    for (u64 i = 0; i < 6; ++i) {
        Token = PFNTokenGetters[i](Str);
        if (Token.Type != _INVALID) { return Token; }
    }

    return Token;
}
