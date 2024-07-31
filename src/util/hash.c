#include "hash.h"

#include <string.h>

#include "../types.h"

u64 HashString(const char* Str) {
    u64 Hash = 0xcbf29ce484222325;

    for (u64 i = 0; i < strlen(Str); ++i) {
        Hash ^= (u8)(Str[i]);
        Hash *= 0x100000001b3;
    }

    return Hash;
}
