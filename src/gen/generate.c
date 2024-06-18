#include "generate.h"

#include <stdio.h>

#include "../parser/node_types.h"
#include "../types.h"

b8 Generate(NodeRoot Tree, FILE* Out) {
    if (!Out) {
        fputs("GenError: Null output parameter.\n", stderr);
        return false;
    }

    fprintf(Out, "global _start\n_start:\n");

    for (u64 i = 0; i < Tree.StatsLen; ++i) {
        NodeStmt Stmt = Tree.Stats[i];

        switch (Stmt.Holds) {
        // exit statement
        case _NODE_STMT_EXIT: {
            if (Stmt.Exit.Holds == _NODE_NUMLIT) {
                fprintf(Out, "      mov rax, 60\n");
                fprintf(Out, "      mov rdi, %s\n", (char*)Stmt.Exit.IntLit.NumLit.Value);
                fprintf(Out, "      syscall\n");
            } else {
                continue;
            }
        } break;

        default: continue;
        }
    }

    return true;
}
