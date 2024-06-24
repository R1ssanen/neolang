#ifndef ERROR_H
#define ERROR_H

#define NO_ERROR NULL

#include <stdio.h>

typedef enum ErrCode {
    _UNKNOWN        = 0,

    _INVALID_ARG    = 0x00000001,
    _INIT_FAIL      = 0x00000002,
    _CLEANUP_FAIL   = 0x00000004,
    _SYNTAX_ERROR   = 0x00000008,
    _SEMANTIC_ERROR = 0x00000010,
    _RUNTIME_ERROR  = 0x00000020
} ErrCode;

typedef struct Error {
    ErrCode Code;
    char*   Msg;
} Error;

Error* _MakeError(ErrCode Code, const char* Type, const char* File, const char* Fmt, ...);

#define ERROR(code, fmt, ...) _MakeError(code, #code, __FILE_NAME__, fmt, ##__VA_ARGS__)

#define PRINT_ERROR(code, fmt, ...)                                                                \
    {                                                                                              \
        Error* Tmp = ERROR(code, fmt, ##__VA_ARGS__);                                              \
        fputs(Tmp->Msg, stderr);                                                                   \
    }

#endif