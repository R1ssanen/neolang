#include "token_types.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sub_types.h"
#include "types.h"

static const char* s_TokenDebugNames[] = {
    [_INVALID] = "_INVALID", [_ID] = "_ID",     [_KEY] = "_KEY",      [_OP] = "_OP",
    [_STRLIT] = "_STRLIT",   [_SPEC] = "_SPEC", [_NUMLIT] = "_NUMLIT"
};

const char* GetTypeDebugName(TokenType Type) { return s_TokenDebugNames[Type]; }

#include "sub_types.h"

Token TryGetKeyword(const char* Str) {
    Token    Token   = { .Subtype = malloc(sizeof(KeyTypes)) };
    KeyTypes Subtype = GetKeySubtype(Str);

    if (Subtype == _KEY_INVALID) { return Token; }

    Token.Type                  = _KEY;
    *(KeyTypes*)(Token.Subtype) = Subtype;

    Token.Value                 = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);
    return Token;
}

Token TryGetOperator(const char* Str) {
    Token Token = { .Subtype = malloc(sizeof(OpTypes)) };

    // hard limit of 1 character
    if (strlen(Str) != 1) { return Token; }

    OpTypes Subtype = GetOperatorSubtype(Str[0]);
    if (Subtype == _OP_INVALID) { return Token; }

    Token.Type                 = _OP;
    *(OpTypes*)(Token.Subtype) = Subtype;

    Token.Value                = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);

    return Token;
}

Token TryGetSpecial(const char* Str) {
    Token     Token   = { .Subtype = malloc(sizeof(SpecTypes)) };
    SpecTypes Subtype = GetSpecialSubtype(Str[0]);

    if (Subtype == _SPEC_INVALID) { return Token; }

    Token.Type                   = _SPEC;
    *(SpecTypes*)(Token.Subtype) = Subtype;

    Token.Value                  = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);

    return Token;
}

Token TryGetNumericLiteral(const char* Str) {
    Token Token                    = { .Subtype = malloc(sizeof(NumLitTypes)) };
    *(NumLitTypes*)(Token.Subtype) = _NUMLIT_INT;

    b8 MultipleDecimalPoints       = false;

    for (u64 i = 0; i < strlen(Str); ++i) {
        if (isdigit(Str[i])) { continue; }

        // sign character allowed
        if (i == 0 && strlen(Str) > 1 && Str[0] == '-') { continue; }

        // floating point value can't have multiple
        // decimal points.
        if (Str[i] == '.' || Str[i] == ',') {
            if (MultipleDecimalPoints) {
                fputs(
                    "TokenError: Floating point numeric literal with multiple decimal "
                    "points.\n",
                    stderr
                );
                return Token;
            }

            MultipleDecimalPoints          = true;
            *(NumLitTypes*)(Token.Subtype) = _NUMLIT_FLOAT;
        } else {
            return Token;
        }
    }

    Token.Value = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);
    Token.Type = _NUMLIT;

    return Token;
}

Token TryGetStringLiteral(const char* Str) {
    // no subtypes
    Token Token = { .Subtype = malloc(1) };

    if (Str[0] == '"' && Str[strlen(Str) - 1] == '"') {
        Token.Value = malloc(strlen(Str) * sizeof(char));
        strcpy((char*)Token.Value, Str);
        Token.Type = _STRLIT;
    }
    return Token;
}

Token TryGetIdentifier(const char* Str) {
    // no subtypes
    Token Token = { .Subtype = malloc(1) };

    if (!isalpha(Str[0])) { return Token; }

    for (u64 i = 0; i < strlen(Str); ++i) {
        if (!isalnum(Str[i])) { return Token; }
    }

    Token.Value = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);
    Token.Type = _ID;

    return Token;
}
