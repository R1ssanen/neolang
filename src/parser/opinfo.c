#include "opinfo.h"

#include "../lexer/token_subtypes.h"
#include "../types.h"

/*OpInfo OPINFOS[] = {
    ['+'] = (OpInfo){ .Assoc = LEFT,  .Prec = 1 },
    ['-'] = (OpInfo){ .Assoc = LEFT,  .Prec = 1 },
    ['*'] = (OpInfo){ .Assoc = LEFT,  .Prec = 2 },
    ['/'] = (OpInfo){ .Assoc = LEFT,  .Prec = 2 },
    ['^'] = (OpInfo){ .Assoc = RIGHT, .Prec = 3 }
};*/

const OpInfo OPINFOS[] = {
    [_OP_ADD] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 1 },
    [_OP_SUB] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 1 },
    [_OP_MUL] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
    [_OP_DIV] = (OpInfo){ .Assoc = _ASSOC_LEFT,  .Prec = 2 },
    [_OP_EXP] = (OpInfo){ .Assoc = _ASSOC_RIGHT, .Prec = 3 }
};
// "=<>~#@"
