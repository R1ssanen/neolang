#include "parse.h"

#include <stdio.h>

#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../types.h"
#include "node_types.h"
#include "parser.h"

static b8 ParseExpr(NodeExpr* Expr) {
    if (Peek(0)->Type == _NUMLIT) {
        Expr->NumLit = (NodeExprNumLit){ .NumLit = *Peek(0) };
        Expr->Holds  = _NODE_NUMLIT;
    }

    else if (Peek(0)->Type == _STRLIT) {
        Expr->StrLit = (NodeExprStrLit){ .StrLit = *Peek(0) };
        Expr->Holds  = _NODE_STRLIT;
    }

    else if (Peek(0)->Type == _ID) {
        Expr->Id    = (NodeExprId){ .Id = *Peek(0) };
        Expr->Holds = _NODE_ID;
    }

    else {
        fprintf(
            stderr, "SyntaxError: Invalid expression; got '%s'.\n", GetTypeDebugName(Peek(0)->Type)
        );
        return false;
    }

    Consume();
    return true;
}

static b8 ParseStmt(NodeStmt* Stmt) {
    // exit statement
    if (Peek(0) && Peek(0)->Type == _KEY && *(KeyTypes*)(Peek(0)->Subtype) == _KEY_EXIT) {
        Consume();

        Stmt->Holds = _NODE_STMT_EXIT;

        // integer literal expression
        if (Peek(0) && Peek(0)->Type == _NUMLIT &&
            *(NumLitTypes*)(Peek(0)->Subtype) == _NUMLIT_INT) {

            Stmt->Exit =
                (NodeStmtExit){ .IntLit = (NodeExprNumLit){ *Peek(0) }, .Holds = _NODE_NUMLIT };
        }

        // identifier
        else if (Peek(0) && Peek(0)->Type == _ID) {
            Stmt->Exit = (NodeStmtExit){ .Id = (NodeExprId){ *Peek(0) }, .Holds = _NODE_ID };
        } else {
            fputs("SyntaxError: Invalid exit statement.\n", stderr);
            return false;
        }

        Consume();
        //    return true;
    }

    // declaration
    else if (Peek(0) && Peek(0)->Type == _BITYPE && Peek(1) && Peek(1)->Type == _SPEC &&
             *(SpecTypes*)(Peek(1)->Subtype) == _SPEC_COLON) {
        Token Type = *Peek(0);
        Consume(); // type
        Consume(); // colon

        b8 IsMutable =
            Peek(0) && Peek(0)->Type == _KEY && *(KeyTypes*)(Peek(0)->Subtype) == _KEY_VAR;
        Stmt->Holds = IsMutable ? _NODE_STMT_MDECL : _NODE_STMT_DECL;

        if (IsMutable) { Consume(); }

        // assignment
        if (Peek(0) && Peek(0)->Type == _ID && Peek(1) && Peek(1)->Type == _OP &&
            *(OpTypes*)(Peek(1)->Subtype) == _OP_EQ) {

            Token Id = *Peek(0);
            Consume(); // id
            Consume(); // equals

            Stmt->Decl = (NodeStmtDecl){ .Type = Type, .Id = Id, .Expr = (NodeExpr){ 0 } };

            if (!ParseExpr(&Stmt->Decl.Expr)) {
                fputs("SyntaxError: Invalid assignment expression.", stderr);
                return false;
            }
        } else {
            fprintf(stderr, "SyntaxError: Invalid declaration; got '%s'.\n", (char*)Peek(0)->Value);
            return false;
        }

        //  return true;
    }

    else {
        fprintf(stderr, "SyntaxError: Invalid statement; got '%s'", (char*)Peek(0)->Value);
        return false;
    }

    // all statements must end in a semicolon
    if (!Peek(0) || !(Peek(0)->Type == _SPEC && *(SpecTypes*)(Peek(0)->Subtype) == _SPEC_SEMI)) {
        fprintf(
            stderr, "SyntaxError: Missing semicolon; got '%s'.\n",
            !Peek(0) ? "" : (char*)Peek(0)->Value
        );
        return false;
    }

    Consume(); // semicolon
    return true;
}

b8 Parse(const Token* Tokens, u64 TokensLen, NodeRoot* Tree) {
    if (!Tokens) {
        fputs("ParseError: Null token input parameter.\n", stderr);
        return false;
    }

    if (!Tree) {
        fputs("ParseError: Null output parameter.\n", stderr);
        return false;
    }

    InitParser(Tokens, TokensLen);

    while (Peek(0)) {

        NodeStmt Stmt = { 0 };
        if (ParseStmt(&Stmt)) {
            Tree->Stats[Tree->StatsLen++] = Stmt;

        } else {
            fprintf(stderr, "SyntaxError: Invalid token '%s'.\n", (char*)Peek(0)->Value);
            return false;
        }
    }

    DestroyParser();
    return true;
}
