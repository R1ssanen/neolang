/**
 * @file util/error.h
 * @brief Basic error structure.
 * @author github.com/r1ssanen
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "../util/arena.h"

typedef enum ErrCode {
    _UNKNOWN        = 0,

    _ASSERT_ERROR   = 0x00000001,
    _RUNTIME_ERROR  = 0x00000002,
    _SYNTAX_ERROR   = 0x00000004,
    _SEMANTIC_ERROR = 0x00000008,
} ErrCode;

typedef struct Error {
    const char* Msg;
    ErrCode     Code;
} Error;

void PrintErrorStack(void);
void _RaiseError(ErrCode Code, const char* Type, const char* Fun, const char* Fmt, ...);

#define RUNTIME_ERR(fmt, ...)                                                                      \
    {                                                                                              \
        _RaiseError(_RUNTIME_ERROR, "Runtime error", __func__, fmt, ##__VA_ARGS__);                \
        exit(EXIT_FAILURE);                                                                        \
    }

#define SYNTAX_ERR(fmt, ...)                                                                       \
    _RaiseError(_SYNTAX_ERROR, "Syntax error", __func__, fmt, ##__VA_ARGS__)

#define SEMANTIC_ERR(fmt, ...)                                                                     \
    _RaiseError(_SEMANTIC_ERROR, "Semantic error", __func__, fmt, ##__VA_ARGS__)

#endif
