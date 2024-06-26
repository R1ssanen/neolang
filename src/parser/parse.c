#include "parse.h"

#include <stdio.h>
#include <string.h>

#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"
#include "node_types.h"
#include "opinfo.h"
#include "parser.h"

Error* ParseTerm(NodeTerm* Term) {
    if (Peek(0) && Peek(0)->Type == _NUMLIT) {
        Term->NumLit      = Alloc(NodeTermNumLit, 1);
        Term->Holds       = _TERM_NUMLIT;
        Term->NumLit->Num = *Peek(0);
    }

    else if (Peek(0) && Peek(0)->Type == _ID) {
        Term->Ident     = Alloc(NodeTermIdent, 1);
        Term->Holds     = _TERM_IDENT;
        Term->Ident->Id = *Peek(0);
    }

    else {
        return ERROR(
            _SYNTAX_ERROR, "Invalid term; got '%s'.", Peek(0) ? (char*)Peek(0)->Value : "null"
        );
    }

    Consume(); // term
    return NO_ERROR;
}

// TODO: move parsing of binary expression to own method
Error* ParseExpr(NodeExpr* Expr, u8 MinPrec) {
    Error* Err = NULL;

    // subexpression
    if (Peek(0) && Peek(0)->Subtype == _SPEC_LPAREN) {
        Consume(); // (

        if ((Err = ParseExpr(Expr, 0))) { return Err; }
        if (!Peek(0) || Peek(0)->Subtype != _SPEC_RPAREN) {
            return ERROR(_SYNTAX_ERROR, "Unmatched parentheses.");
        }

        Consume(); // )
    }

    else {
        Expr->Holds = _TERM;
        Expr->Term  = Alloc(NodeTerm, 1); // initial lhs

        if ((Err = ParseTerm(Expr->Term))) { return Err; }
    }

    while (Peek(0)) {
        // FIX: not every operator is a binary operator
        if (Peek(0)->Type != _OP || OPINFOS[Peek(0)->Subtype].Prec < MinPrec) { break; }

        Token* Token       = Consume(); // op
        OpInfo Info        = OPINFOS[Token->Subtype];
        u8     NextMinPrec = (Info.Assoc == _ASSOC_LEFT) ? Info.Prec + 1 : Info.Prec;

        // operator after operator means
        // the latter marks signedness
        if (Peek(0) && Peek(0)->Subtype == _OP_SUB) {
            Consume(); // -
        }

        NodeExpr* RHS = Alloc(NodeExpr, 1);
        if ((Err = ParseExpr(RHS, NextMinPrec))) { return Err; }

        NodeExpr* NewLHS     = Alloc(NodeExpr, 1);
        NewLHS->Holds        = _BIN_EXPR;
        NewLHS->BinExpr      = Alloc(NodeBinExpr, 1);
        NewLHS->BinExpr->RHS = RHS;
        NewLHS->BinExpr->Op  = Token->Subtype;

        NewLHS->BinExpr->LHS = Alloc(NodeExpr, 1);
        memcpy(NewLHS->BinExpr->LHS, Expr, sizeof(NodeExpr));
        memcpy(Expr, NewLHS, sizeof(NodeExpr));
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

        if ((Err = ParseExpr(Stmt->Exit->Expr, 0))) { return Err; }
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

        if ((Err = ParseExpr(Stmt->Asgn->Expr, 0))) { return Err; }

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

            if ((Err = ParseExpr(Stmt->Def->Expr, 0))) { return Err; }

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
