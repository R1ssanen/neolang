#include "ast_output.h"

#include <stdio.h>
#include <stdlib.h>

#include "../limits.h"
#include "../parser/node_types.h"
#include "../types.h"
#include "../util/assert.h"
#include "jWrite.h"

void OutputIf(const NodeStmtIf* If) {
    jw_object();

    jw_key("expr");
    OutputExpr(If->Expr);

    jw_key("scope");
    OutputScope(If->Scope);

    jwEnd();
}

void OutputTerm(const NodeTerm* Term) {
    jw_object();

    jw_key("holds");
    jw_int(Term->Holds);

    switch (Term->Holds) {
    case _TERM_NUMLIT: {
        jw_key("num_lit");
        jw_double(strtod((char*)Term->NumLit->Num.Value, NULL));
    } break;

    case _TERM_IDENT: {
        jw_key("id");
        jw_string((char*)Term->Ident->Id.Value);
    } break;
    default: break;
    }

    jwEnd();
}

void OutputBinExpr(const NodeBinExpr* BinExpr) {
    jw_object();

    jw_key("lhs");
    OutputExpr(BinExpr->LHS);

    jw_key("rhs");
    OutputExpr(BinExpr->RHS);

    jw_key("op");
    char OpStr[2] = { (char)BinExpr->Op, '\0' };
    jw_string(OpStr);

    jwEnd();
}

void OutputExpr(const NodeExpr* Expr) {
    jw_object();

    jw_key("holds");
    jw_int(Expr->Holds);

    switch (Expr->Holds) {

    case _TERM: {
        jw_key("term");
        OutputTerm(Expr->Term);
    } break;

    case _BIN_EXPR: {
        jw_key("bin_expr");
        OutputBinExpr(Expr->BinExpr);
    } break;

    default: break;
    }

    jwEnd();
}

void OutputInterval(const NodeInterval* Interval) {
    jw_object();

    jw_key("min");
    OutputExpr(Interval->Beg);

    jw_key("max");
    OutputExpr(Interval->End);

    jwEnd();
}

void OutputScope(const NodeScope* Scope) {
    jw_array();
    for (u64 i = 0; i < Scope->StatCount; ++i) { OutputStmt(Scope->Stats[i]); }
    jwEnd();
}

void OutputFor(const NodeStmtFor* For) {
    jw_object();

    jw_key("type");
    jw_int(For->Type);

    jw_key("id");
    jw_string((char*)For->Def->Ident->Id.Value);

    jw_key("scope");
    OutputScope(For->Scope);

    jw_key("range");
    OutputExpr(For->Expr);

    jwEnd();
}

void OutputAsgn(const NodeAsgn* Asgn) {
    jw_object();

    jw_key("id");
    jw_string((char*)Asgn->Ident->Id.Value);

    jw_key("expr");
    OutputExpr(Asgn->Expr);

    jwEnd();
}

void OutputDecl(const NodeDecl* Decl) {
    jw_object();

    jw_key("type");
    jw_int(Decl->Type);

    jw_key("id");
    jw_string((char*)Decl->Ident->Id.Value);

    jwEnd();
}

void OutputDef(const NodeVarDef* Def) {
    jw_object();

    jw_key("type");
    jw_int(Def->Type);

    jw_key("id");
    jw_string((char*)Def->Ident->Id.Value);

    jw_key("expr");
    OutputExpr(Def->Expr);

    jwEnd();
}

void OutputExit(const NodeStmtExit* Exit) {
    jw_object();

    jw_key("expr");
    OutputExpr(Exit->Expr);

    jwEnd();
}

void OutputStmt(const NodeStmt* Stmt) {
    jw_object();

    jw_key("holds");
    jw_int(Stmt->Holds);

    switch (Stmt->Holds) {
    case _STMT_EXIT: {
        jw_key("exit");
        OutputExit(Stmt->Exit);
    } break;
    case _ASGN: {
        jw_key("asgn");
        OutputAsgn(Stmt->Asgn);
    } break;
    case _DECL: {
        jw_key("decl");
        OutputDecl(Stmt->Decl);
    } break;
    case _VAR_DEF: {
        jw_key("def");
        OutputDef(Stmt->VarDef);
    } break;
    case _STMT_FOR: {
        jw_key("for");
        OutputFor(Stmt->For);
    } break;
    case _SCOPE: {
        jw_key("scope");
        OutputScope(Stmt->Scope);
    } break;
    case _STMT_IF: {
        jw_key("if");
        OutputIf(Stmt->If);
    } break;

    default: break;
    }

    jwEnd();
}

void OutputAST(const NodeRoot* Tree, const char* Filename) {
    NASSERT_MSG(Tree, "Null input AST.");
    NASSERT_MSG(Filename, "Null input filename.");

    char AST[MAX_AST_JSON_FILESIZE];
    jwOpen(AST, MAX_AST_JSON_FILESIZE, JW_ARRAY, JW_COMPACT);

    for (u32 i = 0; i < Tree->StatCount; ++i) { OutputStmt(Tree->Stats[i]); }

    jwEnd();
    jwClose();

    FILE* JsonFile = fopen(Filename, "w");
    if (!JsonFile) { RUNTIME_ERR("Could not open output file '%s' for write.", Filename); }

    // puts(AST);
    fputs(AST, JsonFile);

    if (fclose(JsonFile) != 0) { RUNTIME_ERR("Could not close output file '%s'.", Filename); }
}
