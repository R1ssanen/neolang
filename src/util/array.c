#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"

void* _CreateArray(u64 Count, u64 Stride) {
    const u64 HeadSize  = __ARRAY_MAX * sizeof(u64);
    const u64 DataSize  = Count * Stride;
    const u64 TotalSize = HeadSize + DataSize;

    u64*      pBlock    = malloc(TotalSize);
    // automatically zeroed out memory
    memset((void*)pBlock, 0, TotalSize);

    pBlock[_ARRAY_CAP] = Count;
    pBlock[_ARRAY_LEN] = 0;
    pBlock[_ARRAY_STR] = Stride;

    return (void*)(pBlock + __ARRAY_MAX);
}

void _DestroyArray(void* pArray) {
    u64* pFullArray = (u64*)(pArray)-__ARRAY_MAX;
    free(pFullArray);
}

u64 _GetArrayField(void* pArray, u64 Offset) {
    u64* pHead = (u64*)(pArray)-__ARRAY_MAX;
    return pHead[Offset];
}

void _SetArrayField(void* pArray, u64 Offset, u64 Value) {
    u64* pHead    = (u64*)(pArray)-__ARRAY_MAX;
    pHead[Offset] = Value;
}

void* _ResizeArray(void* pArray) {
    u64   Length = GetLengthArray(pArray);
    u64   Stride = GetStrideArray(pArray);

    void* pTemp  = _CreateArray(_ARRAY_RESIZE_FACTOR * GetCapacityArray(pArray), Stride);
    memcpy(pTemp, pArray, Length * Stride);

    _SetArrayField(pTemp, _ARRAY_LEN, Length);
    _DestroyArray(pArray);

    return pTemp;
}

void* _AppendArray(void* pArray, const void* pValue) {
    u64 Length = GetLengthArray(pArray);
    u64 Stride = GetStrideArray(pArray);

    if (Length >= GetCapacityArray(pArray)) { pArray = _ResizeArray(pArray); }

    u64 Address = (u64)pArray;
    Address += Length * Stride;
    memcpy((void*)Address, pValue, Stride);

    _SetArrayField(pArray, _ARRAY_LEN, Length + 1);
    return pArray;
}

void _PopArray(void* pArray, void* pDestination) {
    u64 Length  = GetLengthArray(pArray);
    u64 Stride  = GetStrideArray(pArray);

    u64 Address = (u64)pArray;
    Address += ((Length - 1) * Stride);
    memcpy(pDestination, (void*)Address, Stride);

    _SetArrayField(pArray, _ARRAY_LEN, Length - 1);
}

void* _PopAtArray(void* pArray, u64 Index, void* pDestination) {
    u64 Length = GetLengthArray(pArray);
    u64 Stride = GetStrideArray(pArray);

    if (Index >= Length) {
        fprintf(
            stderr, "ArrayError: Index out of bounds. Length: %lu, Index: %lu\n", Length, Index
        );
        return pArray;
    }

    u64 Address = (u64)pArray;
    memcpy(pDestination, (void*)(Address + (Index * Stride)), Stride);

    if (Index != Length - 1) {
        memcpy(
            (void*)(Address + (Index * Stride)), (void*)(Address + ((Index + 1) * Stride)),
            Stride * (Length - Index)
        );
    }

    _SetArrayField(pArray, _ARRAY_LEN, Length - 1);
    return pArray;
}

void* _InsertArray(void* pArray, u64 Index, void* pValue) {
    u64 Length = GetLengthArray(pArray);
    u64 Stride = GetStrideArray(pArray);

    if (Index >= Length) {
        fprintf(
            stderr, "ArrayError: Index out of bounds. Length: %lu, Index: %lu\n", Length, Index
        );
        return pArray;
    }
    if (Index >= GetCapacityArray(pArray)) { pArray = _ResizeArray(pArray); }

    u64 Address = (u64)pArray;

    if (Index != Length - 1) {
        memcpy(
            (void*)(Address + ((Index + 1) * Stride)), (void*)(Address + (Index * Stride)),
            Stride * (Length - Index)
        );
    }

    memcpy((void*)(Address + (Index * Stride)), pValue, Stride);

    _SetArrayField(pArray, _ARRAY_LEN, Length + 1);
    return pArray;
}
