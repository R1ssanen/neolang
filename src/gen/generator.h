/**
 * @file gen/generator.h
 * @brief Code generators internal state.
 * @author github.com/r1ssanen
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "../types.h"

struct NodeTermIdent;
struct NodeRoot;

// TODO: move somewhere else
typedef struct Variable {
    struct NodeTermIdent* Ident;
    u64                   StackIndex;
    u8                    ByteSize;
} Variable;

typedef struct Generator {
    char*            DATA;
    char*            TEXT;
    char*            ROUT;

    struct NodeRoot* Tree;

    // INFO: maybe allow parser access
    //       to variables?
    Variable* VarRegistry;
    u64       VarCount;
    u64       StackPtr;

    u64*      ScopeRegistry;
    u64       ScopeCount;
} Generator;

extern Generator* State;

void              InitGenerator(const struct NodeRoot* Tree);

#define DataEntry(...)                                                                             \
    {                                                                                              \
        char t[100];                                                                               \
        sprintf(t, __VA_ARGS__);                                                                   \
        strcat(State->DATA, t);                                                                    \
    }

#define TextEntry(...)                                                                             \
    {                                                                                              \
        char t[100];                                                                               \
        sprintf(t, __VA_ARGS__);                                                                   \
        strcat(State->TEXT, t);                                                                    \
    }

#define RoutEntry(...)                                                                             \
    {                                                                                              \
        char t[100];                                                                               \
        sprintf(t, __VA_ARGS__);                                                                   \
        strcat(State->ROUT, t);                                                                    \
    }

void      PushStack(const char* Register);
void      PopStack(const char* Register);

void      RegisterVar(const struct NodeTermIdent* Ident, u8 ByteSize);
Variable* FindVar(const struct NodeTermIdent* Ident);

void      BeginScope(void);
void      EndScope(void);

#endif
