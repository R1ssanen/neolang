/**
 * @file util/arena.h
 * @brief Arena memory allocator.
 * @author github.com/r1ssanen
 */

#ifndef ARENA_H
#define ARENA_H

#include "types.h"

typedef struct Arena {
    void* Block;
    u64   BlockPtr;
    u64   SizeBytes;
} Arena;

b8    InitMemArena(u64 Bytes);

void  DestroyMemArena(void);

void* _Alloc(u64 Stride);

#define Alloc(type, count) (type*)(_Alloc(count * sizeof(type)))

#endif
