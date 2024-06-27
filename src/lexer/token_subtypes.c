#include "token_subtypes.h"

const TokenSubtype KEYSUBTYPES[] = { _KEY_BTRUE, _KEY_BFALSE, _KEY_RET, _KEY_IF,   _KEY_ELIF,
                                     _KEY_ELSE,  _KEY_FOR,    _KEY_VAR, _KEY_EXIT, _KEY_CONT };

const TokenSubtype BISUBTYPES[]  = { _BI_B8,  _BI_I8,  _BI_I16, _BI_I32, _BI_I64, _BI_U8,
                                     _BI_U16, _BI_U32, _BI_U64, _BI_F32, _BI_F64 };
