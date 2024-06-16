/**
 * @file array.h
 * @brief Dynamic array implementation
 * @author github.com/r1ssanen
 */

#ifndef ARRAY_H
#define ARRAY_H

#include "../types.h"

enum {
    _ARRAY_CAP = 0,
    _ARRAY_LEN,
    _ARRAY_STR,

    __ARRAY_MAX
};

void* _CreateArray(u64 Count, u64 Stride);
void  _DestroyArray(void* pArray);

u64   _GetArrayField(void* pArray, u64 Offset);
void  _SetArrayField(void* pArray, u64 Offset, u64 Value);

void* _ResizeArray(void* pArray);

void* _AppendArray(void* pArray, const void* pValue);
void  _PopArray(void* pArray, void* pDestination);

void* _InsertArray(void* pArray, u64 Index, void* pValue);
void* _PopAtArray(void* pArray, u64 Index, void* pDestination);

#define _ARRAY_DEFAULT_CAP      1
#define _ARRAY_RESIZE_FACTOR    2

#define CreateArray(type)       _CreateArray(_ARRAY_DEFAULT_CAP, sizeof(type))
#define ReserveArray(type, cap) _CreateArray(cap, sizeof(type))
#define DestroyArray(array)     _DestroyArray(array)

#define AppendArray(array, value)                                                                  \
    {                                                                                              \
        typeof(value) TempValue = value;                                                           \
        array                   = _AppendArray(array, &TempValue);                                 \
    }

#define PopArray(array, pvalue) _ArrayPop(array, pvalue)

#define InsertArray(array, index, value)                                                           \
    {                                                                                              \
        typeof(value) TempValue = value;                                                           \
        array                   = _InsertArray(array, index, &TempValue);                          \
    }

#define PopAtArray(array, index, pvalue) _PopAtArray(array, index, pvalue)

#define ClearArray(array)                _SetArrayField(array, _ARRAY_LEN, 0)

#define GetCapacityArray(array)          _GetArrayField(array, _ARRAY_CAP)
#define GetLengthArray(array)            _GetArrayField(array, _ARRAY_LEN)
#define GetStrideArray(array)            _GetArrayField(array, _ARRAY_STR)
#define SetLengthArray(array, value)     _SetArrayField(array, _ARRAY_LEN, value)

#endif
