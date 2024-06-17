#ifndef SUB_TYPES_H
#define SUB_TYPES_H

// "+", "-", "*", "/", "=", "<", ">", "~", "#", "@"
typedef enum OpTypes {
    _OP_INVALID  = 0,
    _OP_ADD      = 0x1,
    _OP_SUB      = 0x2,
    _OP_MUL      = 0x4,
    _OP_DIV      = 0x8,
    _OP_EQ       = 0x10,
    _OP_LSHIFT   = 0x20,
    _OP_RSHIFT   = 0x40,
    _OP_ONESCOMP = 0x80,
    _OP_HASH     = 0x100,
    _OP_AT       = 0x200
} OpTypes;

// "[", "]", "(", ")", "{", "}", ",", ":", ";", ".",
// TODO: compound declarations like "->" (declare return type)
typedef enum SpecTypes {
    _SPEC_INVALID = 0,
    _SPEC_LBRACK  = 0x1,
    _SPEC_RBRACK  = 0x2,
    _SPEC_LPAREN  = 0x4,
    _SPEC_RPAREN  = 0x8,
    _SPEC_LBRACE  = 0x10,
    _SPEC_RBRACE  = 0x20,
    _SPEC_COMMA   = 0x40,
    _SPEC_COLON   = 0x80,
    _SPEC_SEMI    = 0x100,
    _SPEC_PERIOD  = 0x200
} SpecTypes;

// "i8", "i32", "true", "false", "ret", "if", "elif", "else", "var",
typedef enum KeyTypes {
    _KEY_INVALID = 0,
    _KEY_INT8    = 0x1,
    _KEY_INT32   = 0x2,
    _KEY_BTRUE   = 0x4,
    _KEY_BFALSE  = 0x8,
    _KEY_RET     = 0x10,
    _KEY_IF      = 0x20,
    _KEY_ELIF    = 0x40,
    _KEY_ELSE    = 0x80,
    _KEY_VAR     = 0x100
} KeyTypes;

typedef enum NumLitTypes {
    _NUMLIT_INVALID = 0,
    _NUMLIT_INT     = 0x1,
    _NUMLIT_FLOAT   = 0x2
} NumLitTypes;

OpTypes   GetOperatorSubtype(const char ch);
SpecTypes GetSpecialSubtype(const char ch);
KeyTypes  GetKeySubtype(const char* Str);

struct Token;
const char* GetSubtypeDebugName(struct Token Token);

#endif
