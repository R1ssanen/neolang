#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "arena.h"

#define MAX_ERRORS 100
Error** ErrorStack;
u64     ErrorsCount;

void    InitErrors(void) {
    ErrorStack  = Alloc(Error*, MAX_ERRORS);
    ErrorsCount = 0;
}

void PrintErrorStack(void) {
    fputs("\nError stack:\n", stderr);
    for (u64 i = 0; i < ErrorsCount; ++i) { fprintf(stderr, "  %s\n", ErrorStack[i]->Msg); }
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
