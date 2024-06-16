#include "sub_types.h"

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

SpecTypes          GetSpecialSubtype(const char ch) { return s_SpecialSymbols[(i32)ch]; }

static const char* s_Keywords[] = {
    "i8", "i32", "true", "false", "ret", "if", "elif", "else", NULL
};

static const KeyTypes s_Keytypes[] = { _KEY_INT8, _KEY_INT32, _KEY_BTRUE, _KEY_BFALSE,
                                       _KEY_RET,  _KEY_IF,    _KEY_ELIF,  _KEY_ELSE };

KeyTypes              GetKeySubtype(const char* Str) {
    for (u64 i = 0; s_Keywords[i]; ++i) {
        if (strcmp(Str, s_Keywords[i]) == 0) { return s_Keytypes[i]; }
    }

    return _KEY_INVALID;
}
