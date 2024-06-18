#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    State      = malloc(sizeof(Generator));

    State->Out = fopen(Filepath, "w");
    if (!State->Out) {
        fprintf(stderr, "GenError: Could not open output file '%s' for write.\n", Filepath);
        return false;
    }

    // assembly entry point
    fprintf(State->Out, "global _start\n_start:\n");

    State->Filepath = calloc(strlen(Filepath), sizeof(char));
    strcpy(State->Filepath, Filepath);

    State->Tree = malloc(sizeof(NodeRoot));
    if (!memcpy((void*)State->Tree, (void*)Tree, sizeof(NodeRoot))) {
        fputs("Error: memcpy failed at 'InitGenerator'.\n", stderr);
        return false;
    }

    const u64 MAX_STACK_VARIABLES = 1000;
    State->Vars                   = malloc(MAX_STACK_VARIABLES * sizeof(Variable));

    State->VarCount               = 0;
    State->StackPtr               = 0;

    return true;
}

b8 DestroyGenerator(void) {
    free((void*)State->Tree);

    if (fclose(State->Out) != 0) {
        fprintf(stderr, "GenError: Could not close output file '%s'.\n", State->Filepath);
        free(State->Filepath);
        return false;
    }

    free(State->Filepath);
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
