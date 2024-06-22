#include "arena.h"

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

static Arena* State = NULL;

b8            InitMemArena(u64 Bytes) {
    if (Bytes == 0) {
        fputs("Cannot initialize memory arena of zero bytes.\n", stderr);
        return false;
    }

    State            = malloc(sizeof(Arena));
    State->Block     = calloc(Bytes, sizeof(char));
    State->BlockPtr  = 0;
    State->SizeBytes = Bytes;

    return true;
}

void DestroyMemArena(void) {
    free(State->Block);
    free(State);
}

static void _Resize(void) {
    // resize factor of 2, every time
    // size is exceeded, memory doubles
    State->SizeBytes *= 2;
    State->Block = realloc(State->Block, State->SizeBytes);
}

void* _Alloc(u64 StrideBytes) {
    u64 Offset = State->BlockPtr;
    while ((Offset + StrideBytes) >= State->SizeBytes) {
        const char* Scale = (State->SizeBytes > 1e6)   ? "mib"
                            : (State->SizeBytes > 1e3) ? "kib"
                                                       : "b";
        // printf("Max arena size exceeded; resizing to %lu %s.\n", State->SizeBytes, Scale);
        _Resize();
    }

    State->BlockPtr += StrideBytes;
    return (void*)((char*)(State->Block) + Offset);
}
