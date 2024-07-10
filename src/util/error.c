#include "error.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../limits.h"
#include "arena.h"

static pthread_mutex_t ErrMutex = PTHREAD_MUTEX_INITIALIZER;
static Error           ErrStack[MAX_ERRORS];
static u64             ErrCount = 0;

void                   PrintErrorStack(void) {
    for (u64 i = 0; i < ErrCount; ++i) { fprintf(stderr, "%s\n", ErrStack[i].Msg); }
}

void _RaiseError(ErrCode Code, const char* Type, const char* Fun, const char* Fmt, ...) {
    char    MsgRoot[MAX_ERROR_MSG_LEN];

    va_list Args;
    va_start(Args, Fmt);
    vsprintf(MsgRoot, Fmt, Args);
    va_end(Args);

    u32   TotalLen = strlen(Type) + strlen(Fun) + strlen(MsgRoot) + 10;
    char* ErrMsg   = Alloc(char, TotalLen);
    sprintf(ErrMsg, "(%s) %s: %s", Fun, Type, MsgRoot);

    pthread_mutex_lock(&ErrMutex);
    ErrStack[ErrCount++] = (Error){ .Msg = ErrMsg, .Code = Code };
    pthread_mutex_unlock(&ErrMutex);
}
