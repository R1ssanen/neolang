/**
 * @file util/hash.h
 * @brief Hash functions.
 * @author github.com/r1ssanen
 */

#ifndef HASH_H
#define HASH_H

#include "../types.h"

// FNV-1a
u64 HashString(const char* Str);

#endif
