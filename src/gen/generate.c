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

static b8 GenerateTerm(const NodeTerm* Term) {

    if (Term->Holds == _TERM_NUMLIT) {
        if (Term->NumLit->Num.Subtype == _NUMLIT_INT) {
            PushStack((char*)Term->NumLit->Num.Value);
            return true;
        }

        char Buf[20];
        sprintf(Buf, "__float32__(%s)", (char*)Term->NumLit->Num.Value);
        PushStack(Buf);
    }

    else if (Term->Holds == _TERM_IDENT) {

        Variable* Var = FindVar(Term->Ident);

        if (!Var) {
            fprintf(
                stderr, "GenError: Variable '%s' doesn't exist.\n", (char*)Term->Ident->Id.Value
            );
            return false;
        }

        if (!Var->Ident->IsInit) {
            fprintf(
                stderr, "GenError: Accessing unitialized variable '%s'.\n",
                (char*)Term->Ident->Id.Value
            );
            return false;
        }

        char VarValue[100];
        sprintf(VarValue, "QWORD [rsp + %i]", (i32)(((State->StackPtr - Var->StackIndex) * 8) - 8));
        PushStack(VarValue);

        return true;
    }

    else {
        fputs("GenError: Invalid term.\n", stderr);
        return false;
    }

    return true;
}

static b8 GenerateExpr(const NodeExpr* Expr) {
    if (!GenerateTerm(Expr->Term)) { fputs("GenError: Could not generate expression.\n", stderr); }

    return true;
}

static b8 GenerateExit(const NodeStmtExit* Exit) {

    if (!GenerateExpr(Exit->Expr)) {
        fputs("GenError: Could not generate expression.\n", stderr);
        return false;
    }

    fprintf(State->Out, "       mov rax, 60\n");
    PopStack("rdi");
    fprintf(State->Out, "       syscall\n");

    return true;
}

static b8 GenerateDef(const NodeStmtDef* Def) {
    Variable* Var = FindVar(Def->Ident);

    if (Var) {
        fprintf(
            stderr, "GenError: Variable '%s' already taken at stack position %lu.\n",
            (char*)Def->Ident->Id.Value, Var->StackIndex
        );
        return false;
    }

    // new variable
    RegisterVar(Def->Ident, BYTESIZE[Def->Type]);

    if (!GenerateExpr(Def->Expr)) { return false; }

    return true;
}

static b8 GenerateDecl(const NodeStmtDecl* Decl) {
    Variable* Var = FindVar(Decl->Ident);

    if (Var) {
        fprintf(
            stderr, "GenError: Variable '%s' already taken at stack position %lu.\n",
            (char*)Decl->Ident->Id.Value, Var->StackIndex
        );
        return false;
    }

    // new variable
    RegisterVar(Decl->Ident, BYTESIZE[Decl->Type]);

    return true;
}

static b8 GenerateAsgn(const NodeStmtAsgn* Asgn) {

    Variable* Var = FindVar(Asgn->Ident);
    if (!Var) {
        fprintf(stderr, "GenError: Variable '%s' doesn't exist.\n", (char*)Var->Ident->Id.Value);
        return false;
    }

    if (!Var->Ident->IsMutable) {
        fprintf(
            stderr, "GenError: Trying to modify constant variable '%s'.\n",
            (char*)Asgn->Ident->Id.Value
        );
        return false;
    }

    if (!GenerateExpr(Asgn->Expr)) { return false; }

    // top of stack contains
    // result of assignment expression.

    if (Var->Ident->IsInit) {
        PopStack("rax");

        // move expression value
        // from rax to variable
        // stack location.
        fprintf(
            State->Out, "       mov [rsp + %i], rax\n",
            (i32)(((State->StackPtr - Var->StackIndex) * 8) - 8)
        );
    }

    else {
        Var->Ident->IsInit = true;
        Var->StackIndex    = State->StackPtr++;
    }

    return true;
}

static b8 GenerateStmt(const NodeStmt* Stmt) {

    if (Stmt->Holds == _STMT_EXIT) {
        if (!GenerateExit(Stmt->Exit)) {
            fputs("GenError: Could not generate code for exit statement.\n", stderr);
            return false;
        }
    }

    else if (Stmt->Holds == _STMT_ASGN) {
        if (!GenerateAsgn(Stmt->Asgn)) {
            fputs("GenError: Could not generate assignment statement.\n", stderr);
            return false;
        }
    }

    else if (Stmt->Holds == _STMT_DECL) {
        if (!GenerateDecl(Stmt->Decl)) {
            fputs("GenError: Could not generate declaration statement.\n", stderr);
            return false;
        }
    }

    else if (Stmt->Holds == _STMT_DEF) {
        if (!GenerateDef(Stmt->Def)) {
            fputs("GenError: Could not generate definition statement.\n", stderr);
            return false;
        }
    }

    else {
        fputs("GenError: Invalid statement.\n", stderr);
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
