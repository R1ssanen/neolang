#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include <string.h>

#include "../types.h"

/**
 * @brief Split a string into two by a delimiter character.
 *
 * @param Str String to split
 * @param Delim Character to split by
 * @param LHS Left-hand side output string
 * @param RHS Right-hand side output string
 * @return False on failure
 */
inline b8 SplitString(const char* Str, const char Delim, char* LHS, char* RHS) {
    if (!Str) {
        fputs("StringError: Cannot split a null string.", stderr);
        return false;
    }
    if (!LHS || !RHS) {
        fputs("StringError: Null output parameter.", stderr);
        return false;
    }

    i64 DelimIndex = -1;
    u32 StrLen     = strlen(Str);
    for (u64 i = 0; i < StrLen; ++i) {
        if (Str[i] == Delim) { DelimIndex = i; }
    }

    // delimiter not found
    if (DelimIndex < 0) { return false; }

    u32  LhsLen = DelimIndex + 1;
    char Lhs[LhsLen];
    strncpy(Lhs, Str, DelimIndex);
    Lhs[DelimIndex] = '\0';

    u32  RhsLen     = StrLen - LhsLen;
    char Rhs[RhsLen];
    strncpy(Rhs, Str + LhsLen, RhsLen - 1);
    Rhs[RhsLen - 1] = '\0';

    strcpy(LHS, Lhs);
    strcpy(RHS, Rhs);
    return true;
}

#endif
