#include "token_types.h"

#include <string.h>

const char*        SPECIAL_SYMBOLS    = "[](){},:;.";
const char*        OPERATORS          = "+-*/^=<>~#@";

const char*        KEYWORDS[]         = { "true", "false", "ret",  "if",   "elif",  "else",
                                          "for",  "var",   "exit", "cont", "while", NULL };

const char*        BITYPES[]          = { "b8",  "i8",  "i16", "i32", "i64", "u8",
                                          "u16", "u32", "u64", "f32", "f64", NULL };

static const char* s_TypeDebugNames[] = {
    [_INVALID] = "_INVALID", [_ID] = "_ID",     [_KEY] = "_KEY",       [_OP] = "_OP",
    [_STRLIT] = "_STRLIT",   [_SPEC] = "_SPEC", [_NUMLIT] = "_NUMLIT", [_BITYPE] = "_BITYPE"
};

const char* GetTypeDebugName(TokenType Type) { return s_TypeDebugNames[Type]; }
