#include "tokenize.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arena.h"
#include "../types.h"
#include "token_subtypes.h"
#include "token_types.h"
#include "tokenizer.h"

b8 Tokenize(const char* Source, u64 SourceLen, Token* Tokens, u64* TokensLen) {
    if (!Source) {
        fputs("TokenError: Tokenization failed, no sources provided.\n", stderr);
        return false;
    }

    if (!Tokens || !TokensLen) {
        fputs("TokenError: Tokenization failed, null output parameter.\n", stderr);
        return false;
    }

    if (!InitTokenizer(Source, SourceLen)) {
        fputs("TokenError: Could not initialize tokenizer.\n", stderr);
        return false;
    }

    // for every bite
    while (PeekBite(0)) {

        // for every character
        while (PeekChar(0)) {

            // skip whitespace
            if (isspace(PeekChar(0))) { ConsumeChar(); }

            // special character
            if (strchr(SPECIAL_SYMBOLS, PeekChar(0))) {
                Token Token            = { .Type    = _SPEC,
                                           .Subtype = (TokenSubtype)(PeekChar(0)),
                                           .Value   = malloc(sizeof(char)) };
                *(char*)(Token.Value)  = PeekChar(0);
                Tokens[(*TokensLen)++] = Token;
                ConsumeChar();
            }

            // numeric literal
            // NOTE: must be before operator
            //       to merge '-' with number
            else if (isdigit(PeekChar(0))) {
                const u64 MAX_NUMLIT_DIGITS = 100;
                char      NumLit[MAX_NUMLIT_DIGITS];
                u64       NumLen  = 0;

                b8        IsFloat = false;
                while (PeekChar(0) && (isdigit(PeekChar(0)) || (PeekChar(0) == '.'))) {
                    if (PeekChar(0) == '.') {
                        if (IsFloat) {
                            fputs(
                                "TokenError: Float literal with multiple decimal points.\n", stderr
                            );
                            return false;
                        }

                        IsFloat = true;
                    }

                    NumLit[NumLen++] = PeekChar(0);
                    ConsumeChar();
                }

                NumLit[NumLen++] = '\0';

                Token Token      = { .Type    = _NUMLIT,
                                     .Subtype = IsFloat ? _NUMLIT_FLOAT : _NUMLIT_INT,
                                     .Value   = Alloc(char, NumLen) };
                strcpy((char*)Token.Value, NumLit);
                Tokens[(*TokensLen)++] = Token;
            }

            // operator
            else if (strchr(OPERATORS, PeekChar(0))) {
                Token Token            = { .Type    = _OP,
                                           .Subtype = (TokenSubtype)(PeekChar(0)),
                                           .Value   = malloc(sizeof(char)) };
                *(char*)(Token.Value)  = PeekChar(0);
                Tokens[(*TokensLen)++] = Token;

                ConsumeChar();
            }

            // string literal
            else if (PeekChar(0) == '"') {
                const u64 MAX_STR_LEN = 1000;
                char      StrLit[MAX_STR_LEN];
                u64       StrLen = 0;

                ConsumeChar(); // first "
                while (PeekChar(0) && PeekChar(0) != '"') {
                    StrLit[StrLen++] = PeekChar(0);
                    ConsumeChar();
                }

                StrLit[StrLen++] = '\0';

                Token Token      = { .Type = _STRLIT, .Value = Alloc(char, StrLen) };
                strcpy((char*)Token.Value, StrLit);
                Tokens[(*TokensLen)++] = Token;

                ConsumeChar(); // second "
            }

            // identifier, keyword
            else if (isalpha(PeekChar(0)) || PeekChar(0) == '_') {
                const u64 MAX_ID_LEN = 100;
                char      Ident[MAX_ID_LEN];
                u64       IdentLen = 0;

                while (PeekChar(0) && (isalnum(PeekChar(0)) || PeekChar(0) == '_')) {
                    Ident[IdentLen++] = PeekChar(0);
                    ConsumeChar();
                }

                Ident[IdentLen++] = '\0';

                Token Token       = { .Type = _ID, .Value = Alloc(char, IdentLen) };
                strcpy((char*)Token.Value, Ident);

                for (u64 i = 0; KEYWORDS[i]; ++i) {
                    if (strcmp(Ident, KEYWORDS[i]) != 0) { continue; }

                    Token.Type    = _KEY;
                    Token.Subtype = KEYSUBTYPES[i];
                }

                Tokens[(*TokensLen)++] = Token;
            }

            else {
                fprintf(stderr, "TokenError: Unknown token in '%s'.\n", PeekBite(0));
                return false;
            }
        }

        ConsumeBite();
    }

    return true;
}
