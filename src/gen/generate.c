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

b8 GenTerm(const NodeTerm* Term) {

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
                _SEMANTIC_ERROR, "Variable '%s' doesn't exist.", (char*)Term->Ident->Id.Value
            );
        }

        if (!Var->Ident->IsInit) {
            return ERROR(
                _SEMANTIC_ERROR, "Accessing uninitialized variable '%s'.",
                (char*)Term->Ident->Id.Value
            );
        }

        char VarValue[100];
        sprintf(VarValue, "QWORD [rsp + %lu]", (State->StackPtr - Var->StackIndex - 1) * 8);
        PushStack(VarValue);
    }

    else {
        THROW_ERROR(_SYNTAX_ERROR, "Invalid term.");
        return false;
    }

    return true;
}

b8 GenFor(const NodeStmtFor* For) {
    static u64 ForID = 0;
    ++ForID;

    if (!GenExpr(For->Interval->Beg)) { return false; }
    if (!GenExpr(For->Interval->End)) { return false; }

    PopStack("rbx"); // end
    PopStack("rax"); // beg

    if (!GenVarDef(For->Def)) { return false; }
    PopStack("rcx"); // iterator

    TextEntry("\t\tmov rcx, rax\n"); // iterator
    TextEntry("\tfor%lu:\n", ForID);

    if (!GenScope(For->Scope)) { return false; }

    TextEntry("\t\tinc rcx\n");
    TextEntry("\t\tcmp rcx, rbx\n");
    TextEntry("\t\tjle for%lu\n", ForID);

    return true;
}

b8 GenBinExpr(const NodeBinExpr* BinExpr) {

    if (!GenExpr(BinExpr->LHS)) { return false; }
    if (!GenExpr(BinExpr->RHS)) { return false; }

    PopStack("rbx"); // rhs in rbx
    PopStack("rax"); // lhs in rax

    switch (BinExpr->Op) {
    case _OP_ADD: TextEntry("\t\tadd rax, rbx\n"); break;
    case _OP_SUB: TextEntry("\t\tsub rax, rbx\n"); break;
    case _OP_MUL: TextEntry("\t\tmul rbx\n"); break;

    case _OP_DIV: {
        TextEntry("\t\txor rdx, rdx\n"); // zero rdx to avoid error
        TextEntry("\t\tdiv rbx\n");
    } break;

    case _OP_EXP: {
        static u8 ExpCount = 0;
        ++ExpCount;

        // pow(rax, rbx)
        // -------------
        // while rbx > 0:
        //  rbx--
        //  rax = rax * rax

        TextEntry("\t\tmov rcx, rax\n"); // save initial lhs value

        TextEntry("\texp%d:\n", ExpCount);
        TextEntry("\t\tdec rbx\n");
        TextEntry("\t\tmul rcx\n");
        TextEntry("\t\tcmp rbx, 1\n");
        TextEntry("\t\tjne exp%d\n", ExpCount);
    } break;

    default: {
        THROW_ERROR(_SYNTAX_ERROR, "Invalid binary expression.");
        return false;
    } break;
    }

    PushStack("rax");
    return true;
}

b8 GenExpr(const NodeExpr* Expr) {

    if (Expr->Holds == _TERM) {
        if (!GenTerm(Expr->Term)) { return false; }
    }

    else if (Expr->Holds == _BIN_EXPR) {
        if (!GenBinExpr(Expr->BinExpr)) { return false; }
    }

    return true;
}

b8 GenScope(const NodeScope* Scope) {
    BeginScope();

    for (u64 i = 0; i < Scope->StatsLen; ++i) {
        if (!GenStmt(Scope->Stats[i])) { return false; }
    }

    return true;

    EndScope();
}

b8 GenExit(const NodeExit* Exit) {
    if (!GenExpr(Exit->Expr)) { return false; }

    TextEntry("\n\t\tmov rax, 60\n");
    PopStack("rdi");
    TextEntry("\t\tsyscall\n");

    return true;
}

b8 GenVarDef(const NodeVarDef* Def) {
    Variable* Var = FindVar(Def->Ident);

    if (Var) {
        THROW_ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Def->Ident->Id.Value, Var->StackIndex
        );
        return false;
    }

    // new variable
    RegisterVar(Def->Ident, BYTESIZE[Def->Type]);
    if (!GenExpr(Def->Expr)) { return false; }
    return true;
}

b8 GenDecl(const NodeDecl* Decl) {
    Variable* Var = FindVar(Decl->Ident);

    if (Var) {
        THROW_ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Decl->Ident->Id.Value, Var->StackIndex
        );
        return false;
    }

    // new variable
    RegisterVar(Decl->Ident, BYTESIZE[Decl->Type]);

    return true;
}

b8 GenAsgn(const NodeStmtAsgn* Asgn) {
    Variable* Var = FindVar(Asgn->Ident);

    if (!Var) {
        THROW_ERROR(_SEMANTIC_ERROR, "Variable '%s' doesn't exist.", (char*)Var->Ident->Id.Value);
        return false;
    }

    if (!Var->Ident->IsMutable) {
        THROW_ERROR(
            _SEMANTIC_ERROR, "Trying to modify constant variable '%s'.",
            (char*)Asgn->Ident->Id.Value
        );
        return false;
    }

    if (!GenExpr(Asgn->Expr)) { return false; }

    if (Var->Ident->IsInit) {
        PopStack("rax");
        TextEntry("\t\tmov [rsp + %lu], rax\n", (State->StackPtr - Var->StackIndex - 1) * 8);
    }

    else {
        Var->Ident->IsInit = true;
        Var->StackIndex    = State->StackPtr++;
    }

    return true;
}

b8 GenStmt(const NodeStmt* Stmt) {

    switch (Stmt->Holds) {

    case _EXIT: {
        if (!GenExit(Stmt->Exit)) { return false; }
    } break;
    case _STMT_ASGN: {
        if (!GenAsgn(Stmt->Asgn)) { return false; }
    } break;
    case _DECL: {
        if (!GenDecl(Stmt->Decl)) { return false; }
    } break;
    case _VAR_DEF: {
        if (!GenVarDef(Stmt->VarDef)) { return false; }
    } break;
    case _SCOPE: {
        if (!GenScope(Stmt->Scope)) { return false; }
    } break;
    case _STMT_FOR: {
        if (!GenFor(Stmt->For)) { return false; }
    } break;

    default: THROW_ERROR(_SYNTAX_ERROR, "Invalid statement."); return false;
    }

    return true;
}

b8 GenRoot(const NodeRoot* Tree) {

    for (u64 i = 0; i < Tree->StatsLen; ++i) {
        NodeStmt* Stmt = Tree->Stats[i];

        if (!GenStmt(Stmt)) { return false; }
    }

    return true;
}

b8 Generate(const NodeRoot* Tree, char* AsmSource) {
    if (!Tree) {
        THROW_ERROR(_INVALID_ARG, "Null input AST.");
        return false;
    }
    if (!AsmSource) {
        THROW_ERROR(_INVALID_ARG, "Null output source string.");
        return false;
    }

    if (!InitGenerator(Tree)) { return false; }
    if (!GenRoot(Tree)) { return false; }

    strcat(AsmSource, State->DATA);
    strcat(AsmSource, State->TEXT);

    return true;
}
