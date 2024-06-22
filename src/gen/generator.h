/**
 * @file gen/generator.h
 * @brief Code generators internal state.
 * @author github.com/r1ssanen
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>

#include "../types.h"

struct NodeTermIdent;

typedef struct Variable {
    struct NodeTermIdent* Ident;
    u64                   StackIndex;
    u8                    ByteSize;
} Variable;

struct NodeRoot;

typedef struct Generator {
    FILE*            Out;
    char*            Filepath;
    struct NodeRoot* Tree;

    Variable*        VarRegistry;
    u64              VarCount;
    u64              StackPtr;
} Generator;

extern Generator* State;

b8                InitGenerator(const char* Filepath, const struct NodeRoot* Tree);
b8                DestroyGenerator(void);

void              PushStack(const char* Register);
void              PopStack(const char* Register);

b8                RegisterVar(const struct NodeTermIdent* Ident, u8 ByteSize);
Variable*         FindVar(const struct NodeTermIdent* Ident);

#endif
