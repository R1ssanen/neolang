#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../limits.h"
#include "../parser/node_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"

Generator* State = NULL;

void       InitGenerator(const NodeRoot* Tree) {
    if (!Tree) { ARG_ERR("Null root node input."); }

    State       = Alloc(Generator, 1);

    // init data section
    State->DATA = Alloc(char, MAX_DATASECT_LEN);
    DataEntry("SECTION .data\n");

    // init text section
    State->TEXT = Alloc(char, MAX_TEXTSECT_LEN);
    TextEntry("\nSECTION .text\nglobal _start\n_start:\n");

    // init routine section
    State->ROUT = Alloc(char, MAX_ROUTSECT_LEN);
    RoutEntry("\n");

    State->VarRegistry   = Alloc(Variable, MAX_STACK_VARS);
    State->Tree          = (NodeRoot*)Tree;
    State->VarCount      = 0;
    State->StackPtr      = 0;

    State->ScopeRegistry = Alloc(u64, MAX_SCOPES);
    State->ScopeCount    = 0;
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
