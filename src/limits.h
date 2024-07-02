#ifndef LIMITS_H
#define LIMITS_H

// errors
#define MAX_ERRORS      254

// lexing
#define MAX_LIT_CHARS   254
#define MAX_TOKENS      100000

// parsing
#define MAX_NODE_STATS  10000
#define MAX_SCOPE_STATS 5000
#define MAX_STACK_VARS  1000
#define MAX_SCOPES      254
#define MAX_FUN_ARGS    100

// gen
#include "types.h"
#define MAX_ASM_FILESIZE      (u64)(1024 * 1024 * 20)        // 20mib
#define MAX_DATASECT_LEN      (u64)(MAX_ASM_FILESIZE * 0.10) // ^10%
#define MAX_TEXTSECT_LEN      (u64)(MAX_ASM_FILESIZE * 0.60) // ^60%
#define MAX_ROUTSECT_LEN      (u64)(MAX_ASM_FILESIZE * 0.30) // ^30%

// visualization
#define MAX_AST_JSON_FILESIZE 1024 * 1024 * 2 // 2mib

#endif
