/**
 * @file util/error.h
 * @brief Basic error structure.
 * @author github.com/r1ssanen
 */

#ifndef ERROR_H
#define ERROR_H

typedef enum ErrCode {
    _UNKNOWN        = 0,

    _ARGUMENT_ERROR = 0x00000001,
    _RUNTIME_ERROR  = 0x00000020,
    _SYNTAX_ERROR   = 0x00000040,
    _SEMANTIC_ERROR = 0x00000080,
} ErrCode;

typedef struct Error {
    ErrCode Code;
    char*   Msg;
} Error;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../util/arena.h"

extern Error* ErrorStack[];
extern u64    ErrorCount;
void          PrintErrorStack(void);

Error*        _MakeError(ErrCode Code, const char* Type, const char* File, const char* Fmt, ...);

#define ARG_ERR(fmt, ...)                                                                          \
    {                                                                                              \
        Error* Tmp =                                                                               \
            _MakeError(_ARGUMENT_ERROR, "Invalid argument", __FILE_NAME__, fmt, ##__VA_ARGS__);    \
        fputs(Tmp->Msg, stderr);                                                                   \
        DestroyMemArena();                                                                         \
        exit(EXIT_FAILURE);                                                                        \
    }

#define RUNTIME_ERR(fmt, ...)                                                                      \
    {                                                                                              \
        Error* Tmp =                                                                               \
            _MakeError(_RUNTIME_ERROR, "Runtime error", __FILE_NAME__, fmt, ##__VA_ARGS__);        \
        fputs(Tmp->Msg, stderr);                                                                   \
        DestroyMemArena();                                                                         \
        exit(EXIT_FAILURE);                                                                        \
    }

#define SYNTAX_ERR(fmt, ...)                                                                       \
    ErrorStack[ErrorCount++] =                                                                     \
        _MakeError(_SYNTAX_ERROR, "Syntax error", __FILE_NAME__, fmt, ##__VA_ARGS__)

#define SEMANTIC_ERR(fmt, ...)                                                                     \
    ErrorStack[ErrorCount++] =                                                                     \
        _MakeError(_SEMANTIC_ERROR, "Semantic error", __FILE_NAME__, fmt, ##__VA_ARGS__)

#endif
