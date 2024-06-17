#include "token_subtypes.h"

#include <string.h>

#include "types.h"

static const OpTypes s_Operators[] = {
    ['+'] = _OP_ADD,    ['-'] = _OP_SUB,    ['*'] = _OP_MUL,      ['/'] = _OP_DIV,  ['='] = _OP_EQ,
    ['<'] = _OP_LSHIFT, ['>'] = _OP_RSHIFT, ['~'] = _OP_ONESCOMP, ['#'] = _OP_HASH, ['@'] = _OP_AT
};

OpTypes                GetOperatorSubtype(const char ch) { return s_Operators[(i32)ch]; }

static const SpecTypes s_SpecialSymbols[] = {
    ['['] = _SPEC_LBRACK, [']'] = _SPEC_RBRACK, ['('] = _SPEC_LPAREN, [')'] = _SPEC_RPAREN,
    ['{'] = _SPEC_LBRACE, ['}'] = _SPEC_RBRACE, [','] = _SPEC_COMMA,  [':'] = _SPEC_COLON,
    [';'] = _SPEC_SEMI,   ['.'] = _SPEC_PERIOD
};

SpecTypes             GetSpecialSubtype(const char ch) { return s_SpecialSymbols[(i32)ch]; }

static const KeyTypes s_Keytypes[] = { _KEY_INT8, _KEY_INT32, _KEY_BTRUE, _KEY_BFALSE, _KEY_RET,
                                       _KEY_IF,   _KEY_ELIF,  _KEY_ELSE,  _KEY_VAR };

#include "token_types.h"

KeyTypes GetKeySubtype(const char* Str) {
    for (u64 i = 0; KEYWORDS[i]; ++i) {
        if (strcmp(Str, KEYWORDS[i]) == 0) { return s_Keytypes[i]; }
    }

    return _KEY_INVALID;
}

static const char* s_OpTypeDebugNames[] = { [_OP_ADD] = "_ADD",       [_OP_SUB] = "_SUB",
                                            [_OP_MUL] = "_MUL",       [_OP_DIV] = "_DIV",
                                            [_OP_EQ] = "_EQ",         [_OP_LSHIFT] = "_LSHIFT",
                                            [_OP_RSHIFT] = "_RSHIFT", [_OP_ONESCOMP] = "_ONESCOMP",
                                            [_OP_HASH] = "_HASH",     [_OP_AT] = "_AT" };

static const char* GetOpTypeDebugName(OpTypes Type) { return s_OpTypeDebugNames[Type]; }

static const char* s_SpecTypeDebugNames[] = {
    [_SPEC_LBRACK] = "_LBRACK", [_SPEC_RBRACK] = "_RBRACK", [_SPEC_LPAREN] = "_LPAREN",
    [_SPEC_RPAREN] = "_RPAREN", [_SPEC_LBRACE] = "_LBRACE", [_SPEC_RBRACE] = "_RBRACE",
    [_SPEC_COMMA] = "_COMMA",   [_SPEC_COLON] = "_COLON",   [_SPEC_SEMI] = "_SEMI",
    [_SPEC_PERIOD] = "_PERIOD"
};

static const char* GetSpecTypeDebugName(SpecTypes Type) { return s_SpecTypeDebugNames[Type]; }

static const char* s_KeyTypeDebugNames[] = {
    [_KEY_INT8] = "_INT8",     [_KEY_INT32] = "_INT32", [_KEY_BTRUE] = "_BTRUE",
    [_KEY_BFALSE] = "_BFALSE", [_KEY_RET] = "_RET",     [_KEY_IF] = "_IF",
    [_KEY_ELIF] = "_ELIF",     [_KEY_ELSE] = "_ELSE",   [_KEY_VAR] = "_VAR"
};

static const char* GetKeyTypeDebugName(KeyTypes Type) { return s_KeyTypeDebugNames[Type]; }

static const char* s_NumLitTypeDebugNames[] = {
    [_NUMLIT_INT] = "_INT", [_NUMLIT_FLOAT] = "_FLOAT"
};

static const char* GetNumLitTypeDebugName(NumLitTypes Type) { return s_NumLitTypeDebugNames[Type]; }

// INFO: quality-of-life abstraction
#include "token_types.h"

const char* GetSubtypeDebugName(Token Token) {
    switch (Token.Type) {

    case _OP: return GetOpTypeDebugName(*(OpTypes*)Token.Subtype);
    case _SPEC: return GetSpecTypeDebugName(*(SpecTypes*)Token.Subtype);
    case _KEY: return GetKeyTypeDebugName(*(KeyTypes*)Token.Subtype);
    case _NUMLIT: return GetNumLitTypeDebugName(*(NumLitTypes*)Token.Subtype);

    default: return "";
    }
}
