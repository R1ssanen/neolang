/**
 * @file util/assert.h
 * @brief Debug assert macros.
 * @author github.com/r1ssanen
 */

#ifndef ASSERT_H
#define ASSERT_H

#include <assert.h>
#include <stdlib.h>

#include "error.h"

#ifdef NDEBUG // no debug
#define NASSERT_MSG(expr, fmt, ...)
#define NASSERT(expr)

#else
#define NASSERT_MSG(expr, fmt, ...)                                                                \
    if (!(expr)) {                                                                                 \
        _RaiseError(_ASSERT_ERROR, "Assertion fail", __func__, fmt, ##__VA_ARGS__);                \
        exit(EXIT_FAILURE);                                                                        \
    }

#define NASSERT(expr) assert((expr))

#endif
#endif
