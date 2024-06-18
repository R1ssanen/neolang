/**
 * @file gen/generator.h
 * @brief Code generators internal state.
 * @author github.com/r1ssanen
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>

#include "../types.h"

typedef struct Variable {
    u64   StackIndex;
    char* Id;
} Variable;

struct NodeRoot;

typedef struct Generator {
    FILE*            Out;
    char*            Filepath;
    struct NodeRoot* Tree;

    Variable*        Vars;
    u64              VarCount;
    u64              StackPtr;
} Generator;

extern Generator* State;

b8                InitGenerator(const char* Filepath, const struct NodeRoot* Tree);
b8                DestroyGenerator(void);

void              PushStack(const char* Register);
void              PopStack(const char* Register);

#endif
