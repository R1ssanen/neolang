#include "arena.h"

#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "assert.h"

static Arena* State = NULL;

void          InitMemArena(u64 Bytes) {
    NASSERT_MSG(Bytes != 0, "Cannot initialize memory arena of zero bytes.");

    State            = malloc(sizeof(Arena));
    State->Block     = calloc(Bytes, sizeof(char));
    State->BlockPtr  = 0;
    State->SizeBytes = Bytes;
}

#define MIB(b) (b / 1048576.f)
#define KIB(b) (b / 1024.f)

void DestroyMemArena(void) {
    puts("\nDestroying memory arena.");
    printf("\t%gKiB (%gMiB) of arena memory used.\n", KIB(State->BlockPtr), MIB(State->BlockPtr));

    f64  TotalUsed = (f64)(State->BlockPtr) / (f64)(State->SizeBytes);
    u32  BarLen    = 35;
    u32  LenUsed   = 0;
    char Bar[BarLen + 1];

    for (; LenUsed < (u32)(BarLen * TotalUsed); ++LenUsed) { Bar[LenUsed] = '#'; }
    for (u32 i = LenUsed; i < BarLen; ++i) { Bar[i] = '-'; }
    Bar[BarLen] = '\0';

    printf("\t[%s] (%u%%)\n", Bar, (u32)(TotalUsed * 100));

    free(State->Block);
    free(State);
}

static void _Resize(u64 Bytes) {
    const f64 RESIZE_FACTOR = 2.0;
    State->SizeBytes += (u64)(Bytes * RESIZE_FACTOR);
    void* NewBlock = realloc(State->Block, State->SizeBytes);

    NASSERT_MSG(NewBlock, "Memory arena resize failure.");
    State->Block = NewBlock;
}

void* _Alloc(u64 Bytes) {
    if ((State->BlockPtr + Bytes) >= State->SizeBytes) {
        _Resize(Bytes);
        fprintf(
            stderr, "Max arena size exceeded; resizing to %gKiB (%gMiB).\n", KIB(State->SizeBytes),
            MIB(State->SizeBytes)
        );
    }

    u64 Tmp = State->BlockPtr;
    State->BlockPtr += Bytes;
    return (void*)((char*)(State->Block) + Tmp);
}
