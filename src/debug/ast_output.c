#include "ast_output.h"

#include <stdio.h>
#include <stdlib.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "jWrite.h"

static void OutputTerm(const NodeTerm* Term) {
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

static void OutputExpr(const NodeExpr* Expr) {
    jw_object();

    jw_key("holds");
    jw_int(Expr->Holds);

    switch (Expr->Holds) {
    case _TERM: {
        jw_key("term");
        OutputTerm(Expr->Term);
    } break;
    default: break;
    }

    jwEnd();
}

static void OutputAsgn(const NodeStmtAsgn* Asgn) {
    jw_object();

    jw_key("id");
    jw_string((char*)Asgn->Ident->Id.Value);

    jw_key("expr");
    OutputExpr(Asgn->Expr);

    jwEnd();
}

static void OutputDecl(const NodeStmtDecl* Decl) {
    jw_object();

    jw_key("type");
    jw_int(Decl->Type);

    jw_key("id");
    jw_string((char*)Decl->Ident->Id.Value);

    jwEnd();
}

static void OutputDef(const NodeStmtDef* Def) {
    jw_object();

    jw_key("type");
    jw_int(Def->Type);

    jw_key("id");
    jw_string((char*)Def->Ident->Id.Value);

    jw_key("expr");
    OutputExpr(Def->Expr);

    jwEnd();
}

static void OutputExit(const NodeStmtExit* Exit) {
    jw_object();

    jw_key("expr");
    OutputExpr(Exit->Expr);

    jwEnd();
}

static void OutputStmt(const NodeStmt* Stmt) {
    jw_object();

    jw_key("holds");
    jw_int(Stmt->Holds);

    switch (Stmt->Holds) {
    case _STMT_EXIT: {
        jw_key("exit");
        OutputExit(Stmt->Exit);
    } break;
    case _STMT_ASGN: {
        jw_key("asgn");
        OutputAsgn(Stmt->Asgn);
    } break;
    case _STMT_DECL: {
        jw_key("decl");
        OutputDecl(Stmt->Decl);
    } break;
    case _STMT_DEF: {
        jw_key("def");
        OutputDef(Stmt->Def);
    } break;
    default: break;
    }

    jwEnd();
}

b8 OutputAST(const NodeRoot* Tree) {
    if (!Tree) {
        fputs("DebugError: Null input parameter.\n", stderr);
        return false;
    }

    char AST[100000];
    jwOpen(AST, 100000, JW_OBJECT, JW_PRETTY);

    jw_key("statements");
    jw_array();
    for (u32 i = 0; i < Tree->StatsLen; ++i) { OutputStmt(Tree->Stats[i]); }
    jwEnd();

    jwEnd();
    jwClose();

    puts("\nAbstract Syntax Tree:");
    printf("\t%s\n", AST);
    putchar('\n');

    FILE* Json = fopen("bin/ast.json", "w");
    if (!Json) {
        fputs("DebugError: Could not open 'bin/ast.json' for write.\n", stderr);
        return false;
    }

    fprintf(Json, "%s", AST);
    fclose(Json);

    return true;
}
