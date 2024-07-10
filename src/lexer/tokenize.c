#include "tokenize.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "../limits.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/assert.h"
#include "../util/error.h"
#include "lexer.h"
#include "token_subtypes.h"
#include "token_types.h"

Token* Tokenize(const char* Source, u64* TokenCount) {
    NASSERT_MSG(Source, "Null source string.");
    NASSERT_MSG(TokenCount, "Null output token count.")

    InitLexer(Source, strlen(Source));
    Token Tokens[MAX_TOKENS];

    while (PeekChar(0)) {
        if ((*TokenCount) >= MAX_TOKENS) {
            RUNTIME_ERR("Max token count of %u exceeded.", MAX_TOKENS);
        }

        if (TryConsumeChar('$')) {
            if (TryConsumeChar('$')) {
                while (PeekChar(0) && ConsumeChar() != '\n'); // single line
            } else {
                while (PeekChar(0) && ConsumeChar() != '$'); // multiline
            }
        }
        if (isspace(PeekChar(0))) {
            ConsumeChar();
            continue;
        }

        // special symbol
        if (strchr(SPECIAL_SYMBOLS, PeekChar(0))) {

            Token Token = { .Type    = _SPEC,
                            .Subtype = (TokenSubtype)(PeekChar(0)),
                            .Value   = Alloc(char, 2),
                            .Line    = GetLine(),
                            .Column  = GetColumn() };

            strcpy((char*)Token.Value, (char[2]){ ConsumeChar(), '\0' });

            Tokens[(*TokenCount)++] = Token;
        }

        // operator
        else if (strchr(OPERATORS, PeekChar(0))) {

            Token Token = { .Type    = _OP,
                            .Subtype = (TokenSubtype)(PeekChar(0)),
                            .Value   = Alloc(char, 2),
                            .Line    = GetLine(),
                            .Column  = GetColumn() };

            strcpy((char*)Token.Value, (char[2]){ ConsumeChar(), '\0' });
            Tokens[(*TokenCount)++] = Token;
        }

        // numeric literal
        else if (isdigit(PeekChar(0))) {

            char NumLit[MAX_LIT_CHARS];
            u64  NumLen     = 0;
            b8   HasDecimal = false;

            while (PeekChar(0) && (isdigit(PeekChar(0)) || (PeekChar(0) == '.'))) {
                if (NumLen >= MAX_LIT_CHARS) {
                    RUNTIME_ERR("Max literal length of %u exceeded.", MAX_LIT_CHARS);
                }

                if (PeekChar(0) == '.') {
                    if (HasDecimal) { break; }
                    HasDecimal = true;
                }

                NumLit[NumLen++] = ConsumeChar();
            }
            NumLit[NumLen++] = '\0';

            Token Token      = { .Type    = _NUMLIT,
                                 .Subtype = HasDecimal ? _NUMLIT_FLOAT : _NUMLIT_INT,
                                 .Value   = Alloc(char, NumLen),
                                 .Line    = GetLine(),
                                 .Column  = GetColumn() };

            strcpy((char*)Token.Value, NumLit);
            Tokens[(*TokenCount)++] = Token;
        }

        // string literal
        else if (TryConsumeChar('"')) {

            char StrLit[MAX_LIT_CHARS];
            u64  StrLen = 0;

            while (PeekChar(0) && PeekChar(0) != '"') {
                if (StrLen >= MAX_LIT_CHARS) {
                    RUNTIME_ERR("Max literal length of %u exceeded.", MAX_LIT_CHARS);
                }

                StrLit[StrLen++] = ConsumeChar();
            }
            ConsumeChar(); // closing "
            StrLit[StrLen++] = '\0';

            Token Token      = { .Type   = _STRLIT,
                                 .Value  = Alloc(char, StrLen),
                                 .Line   = GetLine(),
                                 .Column = GetColumn() };

            strcpy((char*)Token.Value, StrLit);
            Tokens[(*TokenCount)++] = Token;
        }

        // identifier, keyword
        else if (isalpha(PeekChar(0)) || PeekChar(0) == '_') {

            char Ident[MAX_LIT_CHARS];
            u64  IdentLen = 0;

            while (PeekChar(0) && (isalnum(PeekChar(0)) || PeekChar(0) == '_')) {
                if (IdentLen >= MAX_LIT_CHARS) {
                    RUNTIME_ERR("Max literal length of %u exceeded.", MAX_LIT_CHARS);
                }

                Ident[IdentLen++] = ConsumeChar();
            }
            Ident[IdentLen++] = '\0';

            Token Token       = { .Type   = _ID,
                                  .Value  = Alloc(char, IdentLen),
                                  .Line   = GetLine(),
                                  .Column = GetColumn() };

            strcpy((char*)Token.Value, Ident);

            for (u64 i = 0; KEYWORDS[i]; ++i) {
                if (strcmp(Ident, KEYWORDS[i]) != 0) { continue; }

                Token.Type    = _KEY;
                Token.Subtype = KEY_SUBTYPES[i];

                break;
            }

            for (u64 i = 0; BITYPES[i]; ++i) {
                if (strcmp(Ident, BITYPES[i]) != 0) { continue; }

                Token.Type    = _BITYPE;
                Token.Subtype = BITYPE_SUBTYPES[i];

                break;
            }

            Tokens[(*TokenCount)++] = Token;
        }

        else { // unknown token
            ConsumeChar();
        }
    }

    Token* Out = Alloc(Token, (*TokenCount));
    memcpy(Out, Tokens, (*TokenCount) * sizeof(Token));
    return Out;
}
