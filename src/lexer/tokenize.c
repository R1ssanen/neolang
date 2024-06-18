#include "tokenize.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "token_subtypes.h"
#include "token_types.h"

b8 Tokenize(const char* Source, u64 SourceLen, Token* Tokens, u64* TokensLen) {
    if (!Source) {
        fputs("TokenError: Tokenization failed, no sources provided.\n", stderr);
        return false;
    }

    if (!Tokens || !TokensLen) {
        fputs("TokenError: Tokenization failed, null output parameter.\n", stderr);
        return false;
    }

    // strip comments
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
    const u64 MAX_BITES  = 10000;
    char**    Bites      = calloc(MAX_BITES, sizeof(char*));
    u64       TotalBites = 0;

    for (char* Bite = strtok(Buffer, " \t\r\n\v\f"); Bite; Bite = strtok(NULL, " \t\r\n\v\f")) {
        Bites[TotalBites] = malloc(strlen(Bite) * sizeof(char));
        strcpy(Bites[TotalBites], Bite);
        ++TotalBites;
    }

    // resize to actual size
    Bites = realloc(Bites, TotalBites * sizeof(char*));

    for (u64 Curr = 0; Curr < TotalBites; ++Curr) {

        u32  BiteLen = strlen(Bites[Curr]);
        char Bite[BiteLen];
        strcpy(Bite, Bites[Curr]);

        for (u64 i = 0; i < BiteLen; ++i) {

            // is identifier, keyword or builtin-type.
            if (isalpha(Bite[i]) || Bite[i] == '_') {

                // find end of identifier string
                u64 Begin = i;
                while (i < BiteLen && (isalnum(Bite[i + 1]) || Bite[i + 1] == '_')) { ++i; }

                u64   AlnumLen = i - Begin + 1;
                char* AlnumStr = calloc(AlnumLen, sizeof(char));
                strncpy(AlnumStr, Bite + Begin, AlnumLen);

                Token Token = { .Type = _ID, .Value = calloc(AlnumLen, sizeof(char)) };
                strcpy((char*)Token.Value, AlnumStr);

                b8 IsKeyword = false;
                for (u64 i = 0; KEYWORDS[i]; ++i) {
                    if (strcmp(AlnumStr, KEYWORDS[i]) != 0) { continue; }

                    IsKeyword     = true;
                    Token.Type    = _KEY;
                    Token.Subtype = SUBKEYTYPES[i];
                    break;
                }

                if (!IsKeyword) {
                    for (u64 i = 0; BUILTIN_TYPES[i]; ++i) {
                        if (strcmp(AlnumStr, BUILTIN_TYPES[i]) != 0) { continue; }

                        Token.Type    = _BITYPE;
                        Token.Subtype = SUBBITYPES[i];
                        break;
                    }
                }

                free(AlnumStr);

                Tokens[(*TokensLen)++] = Token;
            }

            // is special symbol
            else if (strchr(SPECIAL_SYMBOLS, Bite[i])) {
                Token SpecToken           = { .Type    = _SPEC,
                                              .Subtype = (TokenSubtype)(Bite[i]),
                                              .Value   = calloc(1, sizeof(char)) };
                *(char*)(SpecToken.Value) = Bite[i];
                Tokens[(*TokensLen)++]    = SpecToken;
            }

            // is operator
            else if (strchr(OPERATORS, Bite[i])) {
                Token OpToken           = { .Type    = _OP,
                                            .Subtype = (TokenSubtype)(Bite[i]),
                                            .Value   = calloc(1, sizeof(char)) };
                *(char*)(OpToken.Value) = Bite[i];
                Tokens[(*TokensLen)++]  = OpToken;
            }

            // is string literal
            else if (Bite[i] == '"') {

                // find end of string literal
                // FIX: spaces within strings arent
                //      permitted as the code is split
                //      by whitespace.
                u64 Begin = i;
                while (i < BiteLen && Bite[++i] != '"');

                u64   LitLen      = i - Begin + 1;
                Token StrLitToken = { .Type = _STRLIT, .Value = calloc(LitLen, sizeof(char)) };

                strncpy((char*)StrLitToken.Value, Bite + Begin, LitLen);
                Tokens[(*TokensLen)++] = StrLitToken;
            }

            // is numeric literal
            // TODO: allow dropping of redundant
            //       zero. i.e, .03 is a valid float
            else if (isdigit(Bite[i])) {

                // find end of numeric literal
                u64 Begin        = i;
                b8  DecimalFound = false;
                u64 DecimalIndex = 0;

                while (i < BiteLen && (isdigit(Bite[i + 1]) || Bite[i + 1] == '.')) {
                    if (Bite[i + 1] == '.') {

                        // multiple decimal points
                        if (DecimalFound) {
                            fputs(
                                "TokenError: Numeric literal with multiple decimal points.\n",
                                stderr
                            );
                            return false;
                        }

                        DecimalFound = true;
                        DecimalIndex = i + 1;
                    }
                    ++i;
                }

                u64   ValueLen    = i - Begin + 1;
                Token NumLitToken = { .Type    = _NUMLIT,
                                      .Subtype = _NUMLIT_INT,
                                      .Value   = calloc(ValueLen, sizeof(char)) };

                strncpy((char*)NumLitToken.Value, Bite + Begin, ValueLen);

                // floating point numeric literal
                if (DecimalFound) {
                    NumLitToken.Subtype = _NUMLIT_FLOAT;

                    // if decimal is in last place,
                    // append a zero to make it syntactically correct
                    if (DecimalIndex == (Begin + ValueLen) - 1) {
                        strcat((char*)NumLitToken.Value, "0");
                    }
                }

                Tokens[(*TokensLen)++] = NumLitToken;
            }

            else {
                fprintf(stderr, "TokenError: Unknown token in '%s'.\n", Bite);
                return false;
            }
        }
    }

    free(Bites);
    return true;
}
