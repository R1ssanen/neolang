#include "token_types.h"

#include <ctype.h>
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
    Token    Token   = { 0 };
    KeyTypes SubType = GetKeySubtype(Str);

    if (SubType == _KEY_INVALID) { return Token; }

    Token.BroadType = _KEY;
    Token.SubType   = malloc(sizeof(u32));
    Token.SubType   = (void*)SubType;

    Token.Value     = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);
    return Token;
}

Token TryGetOperator(const char* Str) {
    Token   Token   = { 0 };
    OpTypes SubType = GetOperatorSubtype(Str[0]);

    if (SubType == _OP_INVALID) { return Token; }

    Token.BroadType = _OP;
    Token.SubType   = malloc(sizeof(u32));
    Token.SubType   = (void*)SubType;

    Token.Value     = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);

    return Token;
}

Token TryGetSpecial(const char* Str) {
    Token     Token   = { 0 };
    SpecTypes SubType = GetSpecialSubtype(Str[0]);

    if (SubType == _SPEC_INVALID) { return Token; }

    Token.BroadType = _SPEC;
    Token.SubType   = malloc(sizeof(u32));
    Token.SubType   = (void*)SubType;

    Token.Value     = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);

    return Token;
}

Token TryGetNumericLiteral(const char* Str) {
    Token Token              = { 0 };
    Token.SubType            = malloc(sizeof(u32));
    Token.SubType            = (void*)_NUMLIT_INT;

    b8 MultipleDecimalPoints = false;
    for (u64 i = 0; i < strlen(Str); ++i) {
        if (!isdigit(Str[i])) {
            // floating point value can't have multiple
            // decimal points.
            if (!MultipleDecimalPoints && Str[i] == '.') {
                MultipleDecimalPoints = true;
                Token.SubType         = (void*)_NUMLIT_FLOAT;
            } else {
                return Token;
            }
        }
    }

    Token.Value = malloc(sizeof(u64));
    strcpy((char*)Token.Value, Str);
    Token.BroadType = _NUMLIT;

    return Token;
}

Token TryGetStringLiteral(const char* Str) {
    Token Token = { 0 };
    if (Str[0] == '"' && Str[strlen(Str) - 1] == '"') {
        Token.Value = malloc(strlen(Str) * sizeof(char));
        strcpy((char*)Token.Value, Str);
        Token.BroadType = _STRLIT;
    }
    return Token;
}

Token TryGetIdentifier(const char* Str) {
    Token Token = { 0 };
    if (!isalpha(Str[0])) { return Token; }

    for (u64 i = 0; i < strlen(Str); ++i) {
        if (!isalnum(Str[i])) { return Token; }
    }

    Token.Value = malloc(strlen(Str) * sizeof(char));
    strcpy((char*)Token.Value, Str);
    Token.BroadType = _ID;

    return Token;
}
