#include "generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "generator.h"

static b8 GenerateExpr(const NodeExpr* Expr) {

    if (Expr->Holds == _NODE_NUMLIT) {
        fprintf(State->Out, "       mov rax, %s\n", (char*)Expr->NumLit.NumLit.Value);
        PushStack("rax");
    }

    else if (Expr->Holds == _NODE_STRLIT) {
        // TODO: add strings
    }

    else if (Expr->Holds == _NODE_ID) {

        for (u64 i = 0; i < State->VarCount; ++i) {
            if (strcmp((char*)Expr->Id.Id.Value, State->Vars[i].Id) != 0) { break; }

            // variable identifier exists
            char* Register = malloc(30 * sizeof(char));
            sprintf(
                Register, "QWORD [rsp + %lu]", (State->StackPtr - State->Vars[i].StackIndex) * 4
            );

            PushStack(Register);
            free(Register);

            return true;
        }

        fprintf(
            stderr, "GenError: Variable identifier '%s' doesn't exist.\n", (char*)Expr->Id.Id.Value
        );
        return false;
    }

    else {
        fputs("GenError: Expression type not set.\n", stderr);
        return false;
    }

    return true;
}

static b8 GenerateDecl(const NodeStmtDecl* Decl) {
    for (u64 i = 0; i < State->VarCount; ++i) {

        // variable identifier already taken
        if (strcmp((char*)Decl->Id.Value, State->Vars[i].Id) == 0) {
            fprintf(
                stderr, "GenError: Variable identifier '%s' already taken at stack position %lu.\n",
                (char*)Decl->Id.Value, State->Vars[i].StackIndex
            );
            return false;
        }
    }

    // new variable
    State->Vars[State->VarCount++] =
        (Variable){ .Id = (char*)Decl->Id.Value, .StackIndex = State->StackPtr };

    if (!GenerateExpr(&Decl->Expr)) {
        fputs("GenError: Could not generate code for declaration expression.\n", stderr);
        return false;
    }

    return true;
}

static b8 GenerateExit(const NodeStmtExit* Exit) {

    if (!GenerateExpr(&Exit->Expr)) {
        fputs("GenError: Could not generate source code for expression.\n", stderr);
        return false;
    }

    fprintf(State->Out, "       mov rax, 60\n");
    PopStack("rdi");
    fprintf(State->Out, "       syscall\n");

    return true;
}

static b8 GenerateStmt(const NodeStmt* Stmt) {

    if (Stmt->Holds == _NODE_STMT_EXIT) {
        if (!GenerateExit(&Stmt->Exit)) {
            fputs("GenError: Could not generate code for exit statement.\n", stderr);
            return false;
        }
    }

    else if (Stmt->Holds == _NODE_STMT_DECL) {
        if (!GenerateDecl(&Stmt->Decl)) {
            fputs("GenError: Could not generate code for declaration statement.\n", stderr);
            return false;
        }
    }

    else {
        fputs("GenError: Invalid statement type.\n", stderr);
        return false;
    }

    return true;
}

static b8 GenerateRoot(const NodeRoot* Tree) {

    for (u64 i = 0; i < Tree->StatsLen; ++i) {
        NodeStmt Stmt = Tree->Stats[i];

        if (!GenerateStmt(&Stmt)) {
            fputs("GenError: Could not generate code for statement.\n", stderr);
            return false;
        }
    }

    return true;
}

b8 Generate(const char* Filepath, const NodeRoot* Tree) {
    if (!Filepath) {
        fputs("GenError: Null output filepath.\n", stderr);
        return false;
    }

    if (!Tree) {
        fputs("GenError: Null abstract syntax tree input.\n", stderr);
        return false;
    }

    if (!InitGenerator(Filepath, Tree)) {
        fputs("GenError: Could not initialize code generator.\n", stderr);
        return false;
    }

    if (!GenerateRoot(Tree)) { fputs("GenError: Code generation failed.\n", stderr); }

    if (!DestroyGenerator()) {
        fputs("GenError: Generator cleanup failed.\n", stderr);
        return false;
    }

    return true;
}
