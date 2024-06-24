#include "generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "../util/error.h"
#include "generator.h"

static const char* WORDSIZE[] = { [1] = "BYTE", [2] = "WORD", [4] = "DWORD", [8] = "QWORD" };

static const u8    BYTESIZE[] = {
    [_BI_B8] = 1,  [_BI_I8] = 1,  [_BI_U8] = 1,  [_BI_I16] = 2, [_BI_U16] = 2, [_BI_I32] = 4,
    [_BI_U32] = 4, [_BI_F32] = 4, [_BI_I64] = 8, [_BI_U64] = 8, [_BI_F64] = 8
};

static Error* GenerateTerm(const NodeTerm* Term) {

    if (Term->Holds == _TERM_NUMLIT) {
        if (Term->NumLit->Num.Subtype == _NUMLIT_INT) {
            PushStack((char*)Term->NumLit->Num.Value);

        } else {
            char Buf[20];
            sprintf(Buf, "__float32__(%s)", (char*)Term->NumLit->Num.Value);
            PushStack(Buf);
        }
    }

    else if (Term->Holds == _TERM_IDENT) {

        Variable* Var = FindVar(Term->Ident);

        if (!Var) {
            return ERROR(
                _SEMANTIC_ERROR, "GenError: Variable '%s' doesn't exist.",
                (char*)Term->Ident->Id.Value
            );
        }

        if (!Var->Ident->IsInit) {
            return ERROR(
                _SEMANTIC_ERROR, "GenError: Accessing uninitialized variable '%s'.",
                (char*)Term->Ident->Id.Value
            );
        }

        char VarValue[100];
        sprintf(VarValue, "QWORD [rsp + %i]", (i32)(((State->StackPtr - Var->StackIndex) * 8) - 8));
        PushStack(VarValue);
    }

    else {
        return ERROR(_SYNTAX_ERROR, "Invalid term.");
    }

    return NO_ERROR;
}

static Error* GenerateExpr(const NodeExpr* Expr) {
    Error* Err = GenerateTerm(Expr->Term);
    if (Err) { return Err; }

    return NO_ERROR;
}

static Error* GeneratePut(const NodeStmtPut* Put) {
    static u8 NumStrLits = 0;
    DataEntry("\t\tstr%d db \"%s\", 10\n", NumStrLits, Put->Str);
    DataEntry("\t\tlstr%d equ $-str%d\n", NumStrLits, NumStrLits);

    TextEntry("\t\tmov rax, 1\n");
    TextEntry("\t\tmov rdi, 1\n");
    TextEntry("\t\tmov rsi, str%d\n", NumStrLits);
    TextEntry("\t\tmov rdx, lstr%d\n", NumStrLits);
    TextEntry("\t\tsyscall\n");

    ++NumStrLits;

    return NO_ERROR;
}

static Error* GenerateExit(const NodeStmtExit* Exit) {

    Error* Err = GenerateExpr(Exit->Expr);
    if (Err) { return Err; }

    TextEntry("\t\tmov rax, 60\n");
    PopStack("rdi");
    TextEntry("\t\tsyscall\n");

    return NO_ERROR;
}

static Error* GenerateDef(const NodeStmtDef* Def) {
    Variable* Var = FindVar(Def->Ident);

    if (Var) {
        return ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Def->Ident->Id.Value, Var->StackIndex
        );
    }

    // new variable
    RegisterVar(Def->Ident, BYTESIZE[Def->Type]);

    Error* Err = GenerateExpr(Def->Expr);
    if (Err) { return Err; }

    return NO_ERROR;
}

static Error* GenerateDecl(const NodeStmtDecl* Decl) {
    Variable* Var = FindVar(Decl->Ident);

    if (Var) {
        return ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Decl->Ident->Id.Value, Var->StackIndex
        );
    }

    // new variable
    RegisterVar(Decl->Ident, BYTESIZE[Decl->Type]);

    return NO_ERROR;
}

static Error* GenerateAsgn(const NodeStmtAsgn* Asgn) {

    Variable* Var = FindVar(Asgn->Ident);
    if (!Var) {
        return ERROR(_SEMANTIC_ERROR, "Variable '%s' doesn't exist.", (char*)Var->Ident->Id.Value);
    }

    if (!Var->Ident->IsMutable) {
        return ERROR(
            _SEMANTIC_ERROR, "Trying to modify constant variable '%s'", (char*)Asgn->Ident->Id.Value
        );
    }

    Error* Err = GenerateExpr(Asgn->Expr);
    if (Err) { return Err; }

    if (Var->Ident->IsInit) {
        PopStack("rax");

        // move expression value
        // from rax to variable
        // stack location.
        TextEntry(
            "\t\tmov [rsp + %i], rax\n", (i32)(((State->StackPtr - Var->StackIndex) * 8) - 8)
        );
    }

    else {
        Var->Ident->IsInit = true;
        Var->StackIndex    = State->StackPtr++;
    }

    return NO_ERROR;
}

static Error* GenerateStmt(const NodeStmt* Stmt) {
    Error* Err = NULL;

    switch (Stmt->Holds) {

    case _STMT_EXIT: {
        if ((Err = GenerateExit(Stmt->Exit))) { return Err; }
    } break;
    case _STMT_PUT: {
        if ((Err = GeneratePut(Stmt->Put))) { return Err; }
    } break;
    case _STMT_ASGN: {
        if ((Err = GenerateAsgn(Stmt->Asgn))) { return Err; }
    } break;
    case _STMT_DECL: {
        if ((Err = GenerateDecl(Stmt->Decl))) { return Err; }
    } break;
    case _STMT_DEF: {
        if ((Err = GenerateDef(Stmt->Def))) { return Err; }
    } break;

    default: return ERROR(_SYNTAX_ERROR, "Invalid statement.");
    }

    return NO_ERROR;
}

static Error* GenerateRoot(const NodeRoot* Tree) {
    Error* Err = NULL;

    for (u64 i = 0; i < Tree->StatsLen; ++i) {
        NodeStmt* Stmt = Tree->Stats[i];

        if ((Err = GenerateStmt(Stmt))) { return Err; }
    }

    return NO_ERROR;
}

Error* Generate(const char* Filepath, const NodeRoot* Tree) {
    if (!Filepath) { return ERROR(_INVALID_ARG, "Null output filepath."); }
    if (!Tree) { return ERROR(_INVALID_ARG, "Null input AST."); }

    Error* Err = InitGenerator(Tree);
    if (Err) { return Err; }

    if ((Err = GenerateRoot(Tree))) { return Err; }

    FILE* Asm = fopen(Filepath, "w");
    if (!Asm) {
        return ERROR(_RUNTIME_ERROR, "Could not open output file '%s' for write.", Filepath);
    }

    fprintf(Asm, "%s\n\n", State->DATA);
    fprintf(Asm, "%s", State->TEXT);

    if (fclose(Asm) != 0) {
        return ERROR(_RUNTIME_ERROR, "Could not close output file '%s'.", Filepath);
    }

    return NO_ERROR;
}
