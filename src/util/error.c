#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../limits.h"
#include "arena.h"

Error* ErrorStack[MAX_ERRORS];
u64    ErrorCount = 0;

void   PrintErrorStack(void) {
    for (u64 i = 0; i < ErrorCount; ++i) { fprintf(stderr, "  %s\n", ErrorStack[i]->Msg); }
}

Error* _MakeError(ErrCode Code, const char* Type, const char* File, const char* Fmt, ...) {
    Error* Err = Alloc(Error, 1);
    Err->Code  = Code;

    va_list Ptr;
    va_start(Ptr, Fmt);
    char MsgRoot[1000];
    vsprintf(MsgRoot, Fmt, Ptr);

    u64 TotalLen = strlen(Type) + strlen(File) + strlen(MsgRoot);
    Err->Msg     = Alloc(char, TotalLen);
    sprintf(Err->Msg, "(%s) %s: %s\n", File, Type, MsgRoot);

    return Err;
}
