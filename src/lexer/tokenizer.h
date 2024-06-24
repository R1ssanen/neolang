#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "../types.h"
#include "../util/error.h"

typedef struct Tokenizer {
    char** Bites;
    char*  CurrBite;
    u64    BitesLen;

    u64    BiteIndex;
    u64    CharIndex;
} Tokenizer;

Error* InitTokenizer(const char* Source, u64 SourceLen);

char   PeekChar(u64 Offset);
char   ConsumeChar(void);
char*  PeekBite(u64 Offset);
char*  ConsumeBite(void);

#endif
