#include "generate.h"

#include <assert.h>
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

void GenWhile(const NodeStmtWhile* While) {
    TextEntry("\t\t; while\n");

    BeginScope();

    // if (!GenExpr(While->Expr)) { return false; }

    static u64 WhileID = 0;
    ++WhileID;

    TextEntry("\twhile%lu:\n", WhileID);

    GenScope(While->Scope);
    GenExpr(While->Expr);

    // retrieve loop variable
    PopStack("rax");
    TextEntry("\t\tcmp rax, 1\n");

    // save loop variable
    PushStack("rax");
    TextEntry("\t\tje while%lu\n", WhileID);

    EndScope();
}

void GenFunDef(const NodeFunDef* FunDef) { assert(false && "Unimplemented"); }

void GenFunDecl(const NodeFunDecl* FunDecl) { assert(false && "Unimplented"); }

void GenCall(const NodeCall* Call) { assert(false && "Unimplemented"); }

void GenInterval(const NodeInterval* Interval) {
    TextEntry("\t\t; interval\n");

    GenExpr(Interval->End);
    GenExpr(Interval->Beg);

    PopStack("rax"); // beg
    PopStack("rbx"); // end

    TextEntry("\t\tsub rbx, rax\n");
    PushStack("rbx");
}

void GenIf(const NodeStmtIf* If) {
    TextEntry("\t\t; if\n");

    GenExpr(If->Expr);

    static u64 IfID = 0;
    ++IfID;

    PopStack("rax");
    TextEntry("\t\tcmp rax, 0\n");
    TextEntry("\t\tje if%lu.se\n", IfID);

    TextEntry("\tif%lu:\n", IfID);
    GenScope(If->Scope);
}

void GenTerm(const NodeTerm* Term) {
    TextEntry("\t\t; term\n");

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
            /*THROW_ERROR(
                _SEMANTIC_ERROR, "Variable '%s' doesn't exist.", (char*)Term->Ident->Id.Value
            );*/
            return;
        }

        if (!Var->Ident->IsInit) {
            /*THROW_ERROR(
                _SEMANTIC_ERROR, "Accessing uninitialized variable '%s'.",
                (char*)Term->Ident->Id.Value
            );*/
            return;
        }

        char VarValue[100];
        sprintf(VarValue, "QWORD [rsp + %lu]", (State->StackPtr - Var->StackIndex) * 8);
        PushStack(VarValue);
    }

    else {
        RUNTIME_ERR("Invalid term.");
    }
}

void GenFor(const NodeStmtFor* For) {
    TextEntry("\t\t; for loop\n");

    BeginScope();

    GenVarDef(For->Def);
    GenExpr(For->Expr);

    PopStack("rax"); // range
    PopStack("rcx"); // iterator

    TextEntry("\t\tadd rax, rcx\n");

    // save loop variables
    PushStack("rcx");
    PushStack("rax");

    static u64 ForID = 0;
    ++ForID;

    TextEntry("\tfor%lu:\n", ForID);

    GenScope(For->Scope);

    // retrieve loop variables
    PopStack("rax");
    PopStack("rcx");

    TextEntry("\t\tinc rcx\n");
    TextEntry("\t\tcmp rcx, rax\n");

    // save loop variables
    PushStack("rcx");
    PushStack("rax");

    TextEntry("\t\tjl for%lu\n", ForID);

    EndScope();
}

void GenBinExpr(const NodeBinExpr* BinExpr) {
    TextEntry("\t\t; bin expr\n");

    GenExpr(BinExpr->LHS);
    GenExpr(BinExpr->RHS);

    PopStack("rbx"); // rhs
    PopStack("rax"); // lhs

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

        TextEntry("\texp%d:\n", ExpCount);

        // skip if N is 1
        TextEntry("\t\tcmp rbx, 1\n");
        TextEntry("\t\tje .ee\n");

        TextEntry("\t\tmov rcx, rax\n"); // save initial lhs value
        TextEntry("\t.eb:\n");

        TextEntry("\t\tmul rcx\n");
        TextEntry("\t\tcmp rbx, 1\n");
        TextEntry("\t\tdec rbx\n");
        TextEntry("\t\tjg .eb\n");

        TextEntry("\t.ee:\n");
    } break;

    case _OP_LSHIFT: { // <
        TextEntry("\t\tcmp rax, rbx\n");
        TextEntry("\t\tsetl al\n");
    } break;

    case _OP_RSHIFT: { // >
        TextEntry("\t\tcmp rax, rbx\n");
        TextEntry("\t\tsetg al\n");
    } break;

    default: {
        RUNTIME_ERR("Invalid binary expression.");
    } break;
    }

    PushStack("rax");
}

void GenExpr(const NodeExpr* Expr) {
    TextEntry("\t\t; expr\n");

    switch (Expr->Holds) {
    case _TERM: GenTerm(Expr->Term); break;
    case _BIN_EXPR: GenBinExpr(Expr->BinExpr); break;
    case _INTERVAL: GenInterval(Expr->Interval); break;
    case _ASGN: GenAsgn(Expr->Asgn); break;
    case _CALL: GenCall(Expr->Call); break;

    default: {
        RUNTIME_ERR("Invalid expression.");
    }
    }
}

void GenScope(const NodeScope* Scope) {
    TextEntry("\t\t; scope\n");

    TextEntry("\t.sb:\n");

    for (u64 i = 0; i < Scope->StatCount; ++i) { GenStmt(Scope->Stats[i]); }

    TextEntry("\t.se:\n");
}

void GenExit(const NodeStmtExit* Exit) {
    TextEntry("\t\t; exit\n");

    GenExpr(Exit->Expr);

    TextEntry("\n\t\tmov rax, 60\n");
    PopStack("rdi");
    TextEntry("\t\tsyscall\n");
}

void GenVarDef(const NodeVarDef* Def) {
    TextEntry("\t\t; var def\n");

    Variable* Var = FindVar(Def->Ident);

    if (Var) {
        /*THROW_ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Def->Ident->Id.Value, Var->StackIndex
        );*/
        return;
    }

    GenExpr(Def->Expr);

    // new variable
    RegisterVar(Def->Ident, BYTESIZE[Def->Type]);
}

void GenDecl(const NodeDecl* Decl) {
    TextEntry("\t\t; decl\n");

    Variable* Var = FindVar(Decl->Ident);

    if (Var) {
        /*THROW_ERROR(
            _SEMANTIC_ERROR, "Variable '%s' already taken at stack position %lu.",
            (char*)Decl->Ident->Id.Value, Var->StackIndex
        );*/
        return;
    }

    // new variable
    RegisterVar(Decl->Ident, BYTESIZE[Decl->Type]);
}

void GenAsgn(const NodeAsgn* Asgn) {
    TextEntry("\t\t; assign\n");

    Variable* Var = FindVar(Asgn->Ident);

    if (!Var) {
        // THROW_ERROR(_SEMANTIC_ERROR, "Variable '%s' doesn't exist.",
        // (char*)Var->Ident->Id.Value);
        return;
    }

    if (!Var->Ident->IsMutable) {
        /*THROW_ERROR(
            _SEMANTIC_ERROR, "Trying to modify constant variable '%s'.",
            (char*)Asgn->Ident->Id.Value
        );*/
        return;
    }

    GenExpr(Asgn->Expr);

    if (Var->Ident->IsInit) {
        PopStack("rax");
        TextEntry("\t\tmov [rsp + %lu], rax\n", (State->StackPtr - Var->StackIndex) * 8);
    }

    else {
        Var->Ident->IsInit = true;
        Var->StackIndex    = State->StackPtr++;
    }
}

void GenStmt(const NodeStmt* Stmt) {
    switch (Stmt->Holds) {
    case _ASGN: GenAsgn(Stmt->Asgn); break;
    case _DECL: GenDecl(Stmt->Decl); break;
    case _VAR_DEF: GenVarDef(Stmt->VarDef); break;
    case _SCOPE: GenScope(Stmt->Scope); break;
    case _FUN_DECL: GenFunDecl(Stmt->FunDecl); break;
    case _FUN_DEF: GenFunDef(Stmt->FunDef); break;
    case _CALL: GenCall(Stmt->Call); break;
    case _STMT_EXIT: GenExit(Stmt->Exit); break;
    case _STMT_IF: GenIf(Stmt->If); break;
    case _STMT_FOR: GenFor(Stmt->For); break;
    case _STMT_WHILE: GenWhile(Stmt->While); break;

    default: RUNTIME_ERR("No generator for statement type.");
    }
}

void GenRoot(const NodeRoot* Tree) {
    for (u64 i = 0; i < Tree->StatCount; ++i) {
        NodeStmt* Stmt = Tree->Stats[i];
        GenStmt(Stmt);
    }
}

const char* Generate(const NodeRoot* Tree) {
    if (!Tree) { ARG_ERR("Null input AST."); }

    InitGenerator(Tree);
    GenRoot(Tree);

    char* AsmSource = Alloc(
        char, (strlen(State->DATA) + strlen(State->TEXT) + strlen(State->ROUT)) * sizeof(char)
    );

    strcat(AsmSource, State->DATA);
    strcat(AsmSource, State->TEXT);
    strcat(AsmSource, State->ROUT);

    return AsmSource;
}
