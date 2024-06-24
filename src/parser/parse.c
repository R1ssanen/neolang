#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"
#include "node_types.h"
#include "parser.h"

Error* ParseTerm(NodeTerm* Term) {
    if (Peek(0) && Peek(0)->Type == _NUMLIT) {
        Term->NumLit  = Alloc(NodeTermNumLit, 1);
        *Term->NumLit = (NodeTermNumLit){ .Num = *Peek(0) };
        Term->Holds   = _TERM_NUMLIT;
    }

    else if (Peek(0) && Peek(0)->Type == _ID) {
        Term->Ident  = Alloc(NodeTermIdent, 1);
        *Term->Ident = (NodeTermIdent){ .Id = *Peek(0) };
        Term->Holds  = _TERM_IDENT;
    }

    else {
        return ERROR(
            _SYNTAX_ERROR, "Invalid term; got '%s'.", Peek(0) ? (char*)Peek(0)->Value : "null"
        );
    }

    Consume(); // term
    return NO_ERROR;
}

Error* ParseBinExpr(NodeBinExpr* BinExpr) {
    Error* Err   = NULL;

    BinExpr->LHS = Alloc(NodeExpr, 1);
    if ((Err = ParseExpr(BinExpr->LHS))) { return Err; }

    BinExpr->Op  = Consume()->Subtype; // op

    BinExpr->RHS = Alloc(NodeExpr, 1);
    if ((Err = ParseExpr(BinExpr->RHS))) { return Err; }

    return NO_ERROR;
}

Error* ParseExpr(NodeExpr* Expr) {
    Error* Err = NULL;

    // term
    if (Peek(0) && (Peek(0)->Type == _NUMLIT || Peek(0)->Type == _ID)) {
        Expr->Holds = _TERM;
        Expr->Term  = Alloc(NodeTerm, 1);

        if ((Err = ParseTerm(Expr->Term))) { return Err; }

        // binary expr
        if (Peek(0) && Peek(0)->Type == _OP) {
            Expr->Holds               = _BIN_EXPR;

            // take a copy of term,
            // otherwise expr->binexpr
            // overwrites it.
            NodeTerm* Term            = Expr->Term;

            Expr->BinExpr             = Alloc(NodeBinExpr, 1);
            Expr->BinExpr->Op         = Consume()->Subtype; // op

            Expr->BinExpr->LHS        = Alloc(NodeExpr, 1);
            Expr->BinExpr->LHS->Holds = _TERM;
            Expr->BinExpr->LHS->Term  = Term;

            Expr->BinExpr->RHS        = Alloc(NodeExpr, 1);
            if ((Err = ParseExpr(Expr->BinExpr->RHS))) { return Err; }
        }
    }

    else {
        Expr->Holds   = _BIN_EXPR;
        Expr->BinExpr = Alloc(NodeBinExpr, 1);

        // binary expression
        if ((Err = ParseBinExpr(Expr->BinExpr))) { return Err; }
    }

    return NO_ERROR;
}

Error* ParseStmt(NodeStmt* Stmt) {
    Error* Err = NULL;

    // exit statement, expects [exit]
    if (Peek(0) && Peek(0)->Subtype == _KEY_EXIT) {
        Consume(); // exit

        Stmt->Holds   = _STMT_EXIT;
        Stmt->Exit    = Alloc(NodeStmtExit, 1);
        *(Stmt->Exit) = (NodeStmtExit){ .Expr = Alloc(NodeExpr, 1) };

        if ((Err = ParseExpr(Stmt->Exit->Expr))) { return Err; }
    }

    // print statement, expects [put][str_lit]
    else if (Peek(0) && Peek(0)->Subtype == _KEY_PUT) {
        Consume(); // put

        Stmt->Put      = Alloc(NodeStmtPut, 1);

        Stmt->Holds    = _STMT_PUT;
        Stmt->Put      = Alloc(NodeStmtPut, 1);
        Stmt->Put->Str = Alloc(char, strlen((char*)Peek(0)->Value) + 1);
        strcpy(Stmt->Put->Str, (char*)Peek(0)->Value);

        Consume(); // str_lit
    }

    // assignment, expects [id][=][expr]
    else if (Peek(0) && Peek(0)->Type == _ID && Peek(1) && Peek(1)->Subtype == _OP_EQ) {

        Stmt->Holds = _STMT_ASGN;
        Token Ident = *Peek(0);

        Consume(); // id
        Consume(); // equals

        Stmt->Asgn       = Alloc(NodeStmtAsgn, 1);
        Stmt->Asgn->Expr = Alloc(NodeExpr, 1);

        if ((Err = ParseExpr(Stmt->Asgn->Expr))) { return Err; }

        Stmt->Asgn->Ident    = Alloc(NodeTermIdent, 1);
        *(Stmt->Asgn->Ident) = (NodeTermIdent){ .Id = Ident };
    }

    // definition or declaration, expects [type][:]
    else if (Peek(0) && Peek(0)->Type == _BITYPE && Peek(1) && Peek(1)->Subtype == _SPEC_COLON) {

        Stmt->Holds       = _STMT_DEF;
        TokenSubtype Type = Peek(0)->Subtype;

        Consume(); // type
        Consume(); // colon

        b8 IsMutable = Peek(0) && Peek(0)->Subtype == _KEY_VAR;
        if (IsMutable) {
            Consume(); // var
        }

        Token Ident = *Peek(0);
        Consume(); // id

        // declaration
        if (Peek(0) && Peek(0)->Subtype == _SPEC_SEMI) {
            Consume(); // semi

            Stmt->Holds       = _STMT_DECL;

            Stmt->Decl        = Alloc(NodeStmtDecl, 1);
            Stmt->Decl->Type  = Type;
            Stmt->Decl->Ident = Alloc(NodeTermIdent, 1);
            *(Stmt->Decl->Ident) =
                (NodeTermIdent){ .Id = Ident, .IsMutable = IsMutable, .IsInit = false };

            return NO_ERROR;
        }

        // definition, expects [=][expr]
        else if (Peek(0) && Peek(0)->Subtype == _OP_EQ) {
            Consume(); // equals

            Stmt->Def       = Alloc(NodeStmtDef, 1);
            Stmt->Def->Expr = Alloc(NodeExpr, 1);

            if ((Err = ParseExpr(Stmt->Def->Expr))) { return Err; }

            Stmt->Def->Type  = Type;
            Stmt->Def->Ident = Alloc(NodeTermIdent, 1);
            *(Stmt->Decl->Ident) =
                (NodeTermIdent){ .Id = Ident, .IsMutable = IsMutable, .IsInit = true };
        }

        else {
            return ERROR(_SYNTAX_ERROR, "Invalid statement.");
        }
    }

    else {
        return ERROR(
            _SYNTAX_ERROR, "Invalid statement; got '%s'.", Peek(0) ? (char*)Peek(0)->Value : "null"
        );
    }

    // no semicolon to end
    if (!Peek(0) || !(Peek(0)->Subtype == _SPEC_SEMI)) {
        return ERROR(
            _SYNTAX_ERROR, "Missing semicolon; got '%s'.", Peek(0) ? (char*)Peek(0)->Value : "null"
        );
    }

    Consume(); // semicolon
    return NO_ERROR;
}

Error* ParseRoot(NodeRoot* Tree) {
    Error* Err = NULL;

    while (Peek(0)) {
        NodeStmt* Stmt = Alloc(NodeStmt, 1);

        if ((Err = ParseStmt(Stmt))) {
            return Err;
        } else {
            Tree->Stats[Tree->StatsLen++] = Stmt;
        }
    }

    return NO_ERROR;
}

Error* Parse(const Token* Tokens, u64 TokensLen, NodeRoot* Tree) {
    if (!Tokens) { return ERROR(_INVALID_ARG, "No tokens to parse."); }
    if (!Tree) { return ERROR(_INVALID_ARG, "Null output parameter."); }

    Error* Err = InitParser(Tokens, TokensLen);
    if (Err) { return Err; }

    if ((Err = ParseRoot(Tree))) { return Err; }

    return NO_ERROR;
}
