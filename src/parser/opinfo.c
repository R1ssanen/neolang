#include "opinfo.h"

#include "../lexer/token_subtypes.h"

const OpInfo OPINFOS[] = {
    // arithmetic binary
    [_OP_ADD]    = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 1 },
    [_OP_SUB]    = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 1 },
    [_OP_MUL]    = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
    [_OP_DIV]    = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
    [_OP_EXP]    = (OpInfo){ .Assoc = _ASSOC_RIGHT, .Prec = 3 },

    // boolean binary
    [_OP_LSHIFT] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
    [_OP_RSHIFT] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
};
// "=<>~#@"
