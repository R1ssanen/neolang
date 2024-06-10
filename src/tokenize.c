#include "tokenize.h"

#include <stdio.h>
#include <string.h>

#include "types.h"

static const char* s_TokenDebugNames[__MAX] = { "_ID", "_KEY", "_OP", "_STR", "_SPEC", "_CONST" };

const char*        GetTokenDebugName(Token Token) { return s_TokenDebugNames[(u32)(Token)]; }

b8 Tokenize(const char* pSource, u64 SourceLen, Token* pOutTokens, u64* pOutTokensLen) {
    if (!pSource) {
        fputs("Tokenization failed, no sources provided.", stderr);
        return false;
    }

    if (!pOutTokens || !pOutTokensLen) {
        fputs("Tokenization failed, null output parameter.", stderr);
        return false;
    }

    char pBuffer[SourceLen];
    strncpy_s(pBuffer, SourceLen, pSource, SourceLen);

    for (char* pCurr = pBuffer; pCurr < pBuffer + SourceLen; ++pCurr) { putchar(*pCurr); }

    return true;
}
