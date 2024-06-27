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
    _OP_EXP       = '^',
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
    _KEY_FOR      = 0x00010000,
    _KEY_CONT     = 0x00020000,

    // built-in types
    _BI_B8        = 0x00100000,
    _BI_I8        = 0x00200000,
    _BI_I16       = 0x00400000,
    _BI_I32       = 0x00800000,
    _BI_I64       = 0x01000000,
    _BI_U8        = 0x02000000,
    _BI_U16       = 0x04000000,
    _BI_U32       = 0x08000000,
    _BI_U64       = 0x10000000,
    _BI_F32       = 0x20000000,
    _BI_F64       = 0x40000000,

    // numeric literals
    _NUMLIT_INT   = 0x40000001,
    _NUMLIT_FLOAT = 0x40000002,
    _NUMLIT_DOUBL = 0x40000004
} TokenSubtype;

// "+", "-", "*", "/", "=", "<", ">", "~", "#", "@"
// "[", "]", "(", ")", "{", "}", ",", ":", ";", ".",
// "true", "false", "ret", "if", "elif", "else", "var", "exit", "put"

extern const TokenSubtype KEYSUBTYPES[];
extern const TokenSubtype BISUBTYPES[];

#endif
