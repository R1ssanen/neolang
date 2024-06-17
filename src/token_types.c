#include "token_types.h"

#include <string.h>

const char* SPECIAL_SYMBOLS = "[](){},:;.";
const char* OPERATORS       = "+-*/=<>~#@";
const char* KEYWORDS[] = { "i8", "i32", "true", "false", "ret", "if", "elif", "else", "var", NULL };

static const char* s_TypeDebugNames[] = {
    [_INVALID] = "_INVALID", [_ID] = "_ID",     [_KEY] = "_KEY",      [_OP] = "_OP",
    [_STRLIT] = "_STRLIT",   [_SPEC] = "_SPEC", [_NUMLIT] = "_NUMLIT"
};

const char* GetTypeDebugName(TokenType Type) { return s_TypeDebugNames[Type]; }
