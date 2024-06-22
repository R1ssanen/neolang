/**
 * @file lexer/token_subtypes.h
 * @brief Token subtype enumerations.
 * @author github.com/r1ssanen
 */

#ifndef TOKEN_SUBTYPES_H
#define TOKEN_SUBTYPES_H

typedef enum TokenSubtype {
    _SUB_INVALID  = 0,

    // operators
    _OP_ADD       = '+',
    _OP_SUB       = '-',
    _OP_MUL       = '*',
    _OP_DIV       = '/',
    _OP_EQ        = '=',
    _OP_LSHIFT    = '<',
    _OP_RSHIFT    = '>',
    _OP_ONESCOMP  = '^',
    _OP_HASH      = '#',
    _OP_AT        = '@',

    // special symbols
    _SPEC_LBRACK  = '[',
    _SPEC_RBRACK  = ']',
    _SPEC_LPAREN  = '(',
    _SPEC_RPAREN  = ')',
    _SPEC_LBRACE  = '{',
    _SPEC_RBRACE  = '}',
    _SPEC_PERIOD  = '.',
    _SPEC_COMMA   = ',',
    _SPEC_COLON   = ':',
    _SPEC_SEMI    = ';',

    // keywords
    _KEY_BTRUE    = 0x00000100,
    _KEY_BFALSE   = 0x00000200,
    _KEY_RET      = 0x00000400,
    _KEY_IF       = 0x00000800,
    _KEY_ELIF     = 0x00001000,
    _KEY_ELSE     = 0x00002000,
    _KEY_VAR      = 0x00004000,
    _KEY_EXIT     = 0x00008000,

    // built-in types
    _BI_B8        = 0x000010000,
    _BI_I8        = 0x000020000,
    _BI_I16       = 0x000040000,
    _BI_I32       = 0x000080000,
    _BI_I64       = 0x000100000,
    _BI_U8        = 0x000200000,
    _BI_U16       = 0x000400000,
    _BI_U32       = 0x000800000,
    _BI_U64       = 0x001000000,
    _BI_F32       = 0x002000000,
    _BI_F64       = 0x004000000,

    // numeric literals
    _NUMLIT_INT   = 0x008000000,
    _NUMLIT_FLOAT = 0x010000000,
    _NUMLIT_DOUBL = 0x020000000
} TokenSubtype;

// "+", "-", "*", "/", "=", "<", ">", "~", "#", "@"
// "[", "]", "(", ")", "{", "}", ",", ":", ";", ".",
// "true", "false", "ret", "if", "elif", "else", "var", "exit"

extern const TokenSubtype KEYSUBTYPES[];

#endif