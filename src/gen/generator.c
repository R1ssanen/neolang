#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../arena.h"
#include "../parser/node_types.h"
#include "../types.h"

Generator* State = NULL;

b8         InitGenerator(const char* Filepath, const NodeRoot* Tree) {
    if (!Filepath) {
        fputs("GenError: Null output filepath.\n", stderr);
        return false;
    }

    if (!Tree) {
        fputs("GenError: Null root node input.\n", stderr);
        return false;
    }

    State      = Alloc(Generator, 1);

    State->Out = fopen(Filepath, "w");
    if (!State->Out) {
        fprintf(stderr, "GenError: Could not open output file '%s' for write.\n", Filepath);
        return false;
    }

    // assembly entry point
    fprintf(State->Out, "global _start\n_start:\n");

    State->Filepath = Alloc(char, strlen(Filepath));
    strcpy(State->Filepath, Filepath);

    State->Tree = Alloc(NodeRoot, 1);
    if (!memcpy((void*)State->Tree, (void*)Tree, sizeof(NodeRoot))) {
        fputs("Error: memcpy failed at 'InitGenerator'.\n", stderr);
        return false;
    }

    const u64 MAX_STACK_VARIABLES = 1000;
    State->VarRegistry            = Alloc(Variable, MAX_STACK_VARIABLES);

    State->VarCount               = 0;
    State->StackPtr               = 0;

    return true;
}

b8 DestroyGenerator(void) {
    if (fclose(State->Out) != 0) {
        fprintf(stderr, "GenError: Could not close output file '%s'.\n", State->Filepath);
        return false;
    }

    State = NULL;
    return true;
}

void PushStack(const char* Register) {
    fprintf(State->Out, "       push %s\n", Register);
    ++State->StackPtr;
}

void PopStack(const char* Register) {
    fprintf(State->Out, "       pop %s\n", Register);
    --State->StackPtr;
}

b8 RegisterVar(const NodeTermIdent* Ident, u8 ByteSize) {
    State->VarRegistry[State->VarCount++] = (Variable
    ){ .Ident = (NodeTermIdent*)Ident, .StackIndex = State->StackPtr, .ByteSize = ByteSize };

    return true;
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
