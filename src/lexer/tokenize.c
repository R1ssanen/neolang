#include "tokenize.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"
#include "token_subtypes.h"
#include "token_types.h"
#include "tokenizer.h"

Error* Tokenize(const char* Source, u64 SourceLen, Token* Tokens, u64* TokensLen) {
    if (!Source) { return ERROR(_INVALID_ARG, "No source string to tokenize."); }
    if (!Tokens || !TokensLen) { return ERROR(_INVALID_ARG, "Null output paratemers."); }

    Error* Err = InitTokenizer(Source, SourceLen);
    if (Err) { return Err; }

    while (PeekBite(0)) {

        while (PeekChar(0)) {

            // skip whitespace
            if (isspace(PeekChar(0))) { ConsumeChar(); }

            // special character
            if (strchr(SPECIAL_SYMBOLS, PeekChar(0))) {
                Token Token = { .Type    = _SPEC,
                                .Subtype = (TokenSubtype)(PeekChar(0)),
                                .Value   = Alloc(char, 2) };

                strcpy((char*)Token.Value, (char[2]){ PeekChar(0), '\0' });

                Tokens[(*TokensLen)++] = Token;
                ConsumeChar();
            }

            // operator
            else if (strchr(OPERATORS, PeekChar(0))) {
                Token Token = { .Type    = _OP,
                                .Subtype = (TokenSubtype)(PeekChar(0)),
                                .Value   = Alloc(char, 2) };
                strcpy((char*)Token.Value, (char[2]){ PeekChar(0), '\0' });

                Tokens[(*TokensLen)++] = Token;
                ConsumeChar();
            }

            // numeric literal
            else if (isdigit(PeekChar(0))) {
                const u64 MAX_NUMLIT_DIGITS = 100;
                char      NumLit[MAX_NUMLIT_DIGITS];
                u64       NumLen  = 0;

                b8        IsFloat = false;

                while (PeekChar(0) && (isdigit(PeekChar(0)) || (PeekChar(0) == '.'))) {
                    if (PeekChar(0) == '.') {
                        if (IsFloat) {
                            return ERROR(
                                _SYNTAX_ERROR, "Float literal with multiple decimal points."
                            );
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

            // string literal
            else if (PeekChar(0) == '"') {
                const u64 MAX_STR_LEN = 1000;
                char      StrLit[MAX_STR_LEN];
                u64       StrLen = 0;

                ConsumeChar(); // opening "
                while (PeekChar(0) && PeekChar(0) != '"') { StrLit[StrLen++] = ConsumeChar(); }
                ConsumeChar(); // closing "

                StrLit[StrLen++] = '\0';

                Token Token      = { .Type = _STRLIT, .Value = Alloc(char, StrLen) };
                strcpy((char*)Token.Value, StrLit);
                Tokens[(*TokensLen)++] = Token;

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

                    break;
                }

                for (u64 i = 0; BITYPES[i]; ++i) {
                    if (strcmp(Ident, BITYPES[i]) != 0) { continue; }

                    Token.Type    = _BITYPE;
                    Token.Subtype = BISUBTYPES[i];

                    break;
                }

                Tokens[(*TokensLen)++] = Token;
            }

            else {
                return ERROR(_SYNTAX_ERROR, "Invalid token in '%s'.", PeekBite(0));
            }
        }

        ConsumeBite();
    }

    return NO_ERROR;
}
