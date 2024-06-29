#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

Generator* State = NULL;

b8         InitGenerator(const NodeRoot* Tree) {
    if (!Tree) {
        THROW_ERROR(_INVALID_ARG, "Null root node input.");
        return false;
    }

    State                      = Alloc(Generator, 1);

    // initialize data section
    const u64 MAX_DATASECT_LEN = 10000;
    State->DATA                = Alloc(char, MAX_DATASECT_LEN);
    DataEntry("SECTION .data\n");

    // initialize text section
    const u64 MAX_TEXTSECT_LEN = 1000000;
    State->TEXT                = Alloc(char, MAX_TEXTSECT_LEN);
    TextEntry("\nSECTION .text\nglobal _start\n_start:\n");

    const u64 MAX_STACK_VARIABLES = 1000;
    State->VarRegistry            = Alloc(Variable, MAX_STACK_VARIABLES);
    State->Tree                   = (NodeRoot*)Tree;
    State->VarCount               = 0;
    State->StackPtr               = 0;

    const u64 MAX_SCOPES          = 100;
    State->ScopeRegistry          = Alloc(u64, MAX_SCOPES);
    State->ScopeCount             = 0;

    return true;
}

void PushStack(const char* Register) {
    TextEntry("\t\tpush %s\n", Register);
    ++State->StackPtr;
}

void PopStack(const char* Register) {
    TextEntry("\t\tpop %s\n", Register);
    --State->StackPtr;
}

void RegisterVar(const NodeTermIdent* Ident, u8 ByteSize) {
    State->VarRegistry[State->VarCount++] = (Variable
    ){ .Ident = (NodeTermIdent*)Ident, .StackIndex = State->StackPtr, .ByteSize = ByteSize };
}

Variable* FindVar(const struct NodeTermIdent* Ident) {
    const char* Id = (char*)Ident->Id.Value;

    for (u64 i = 0; i < State->VarCount; ++i) {
        if (strcmp(Id, (char*)State->VarRegistry[i].Ident->Id.Value) == 0) {
            return State->VarRegistry + i;
        }
    }

    return NULL;
}

void BeginScope(void) { State->ScopeRegistry[State->ScopeCount++] = State->VarCount; }

void EndScope(void) {
    u64 ScopeVarCount = State->VarCount - State->ScopeRegistry[State->ScopeCount - 1];

    TextEntry("\t\tadd rsp, %lu\n", ScopeVarCount * 8);

    for (u64 i = (State->VarCount - ScopeVarCount); i < State->VarCount; ++i) {
        // zero out variable memory
        memset(State->VarRegistry + i, 0, sizeof(Variable));
    }

    State->VarCount -= ScopeVarCount;
    State->StackPtr -= ScopeVarCount;
    State->ScopeRegistry[State->ScopeCount - 1] = 0;
    --State->ScopeCount;
}
