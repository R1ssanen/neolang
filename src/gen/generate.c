#include "generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "generator.h"

static const char* WORDSIZE[] = { [1] = "BYTE", [2] = "WORD", [4] = "DWORD", [8] = "QWORD" };

static const u8    BYTESIZE[] = {
    [_BI_B8] = 1,  [_BI_I8] = 1,  [_BI_U8] = 1,  [_BI_I16] = 2, [_BI_U16] = 2, [_BI_I32] = 4,
    [_BI_U32] = 4, [_BI_F32] = 4, [_BI_I64] = 8, [_BI_U64] = 8, [_BI_F64] = 8
};

static b8 GenerateExpr(const NodeExpr* Expr) {

    if (Expr->Holds == _NODE_NUMLIT) {
        if (Expr->NumLit->Num.Subtype == _NUMLIT_INT) {
            PushStack((char*)Expr->NumLit->Num.Value);
            return true;
        }

        char Buf[20];
        sprintf(Buf, "__float32__(%s)", (char*)Expr->NumLit->Num.Value);
        PushStack(Buf);
    }

    else if (Expr->Holds == _NODE_STRLIT) {
        // TODO: add strings
    }

    else if (Expr->Holds == _NODE_ID) {

        for (u64 i = 0; i < State->VarCount; ++i) {
            const Variable* Var = &State->VarRegistry[i];

            if (strcmp((char*)Expr->Ident->Id.Value, Var->Ident->Id.Value) != 0) { continue; }

            // printf("Var: %-7s Index: %-7lu\n", (char*)Var->Ident->Id.Value, Var->StackIndex);

            // variable identifier exists
            char VarValue[100];
            sprintf(
                VarValue, "QWORD [rsp + %i]", (i32)(((State->StackPtr - Var->StackIndex) * 8) - 8)
            );

            PushStack(VarValue);

            return true;
        }

        fprintf(
            stderr, "GenError: Variable identifier '%s' doesn't exist.\n",
            (char*)Expr->Ident->Id.Value
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
        if (strcmp((char*)Decl->Ident->Id.Value, State->VarRegistry[i].Ident->Id.Value) == 0) {
            fprintf(
                stderr, "GenError: Variable identifier '%s' already taken at stack position %lu.\n",
                (char*)Decl->Ident->Id.Value, State->VarRegistry[i].StackIndex
            );
            return false;
        }
    }

    // new variable
    RegisterVar(Decl->Ident, BYTESIZE[Decl->Type]);

    if (!GenerateExpr(Decl->Expr)) {
        fputs("GenError: Could not generate code for declaration expression.\n", stderr);
        return false;
    }

    return true;
}

static b8 GenerateExit(const NodeStmtExit* Exit) {

    if (!GenerateExpr(Exit->Expr)) {
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
        if (!GenerateExit(Stmt->Exit)) {
            fputs("GenError: Could not generate code for exit statement.\n", stderr);
            return false;
        }
    }

    else if (Stmt->Holds == _NODE_STMT_DECL) {
        if (!GenerateDecl(Stmt->Decl)) {
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
        NodeStmt* Stmt = Tree->Stats[i];

        if (!GenerateStmt(Stmt)) {
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
