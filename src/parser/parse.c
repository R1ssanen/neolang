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
        fprintf(stderr, "SyntaxError: Invalid expression; got '%s'.\n", (char*)Peek(0)->Value);
        return false;
    }

    Consume();
    return true;
}

static b8 ParseStmt(NodeStmt* Stmt) {
    // exit statement, expects [exit]
    if (Peek(0) && Peek(0)->Subtype == _KEY_EXIT) {
        Consume(); // exit

        Stmt->Holds = _NODE_STMT_EXIT;

        Stmt->Exit  = (NodeStmtExit){ .Expr = { 0 } };
        if (!ParseExpr(&Stmt->Exit.Expr)) {
            fputs("SyntaxError: Invalid expression.\n", stderr);
            return false;
        }
    }

    // declaration, expects [type][:]
    else if (Peek(0) && Peek(0)->Type == _BITYPE && Peek(1) && Peek(1)->Subtype == _SPEC_COLON) {

        Stmt->Holds = _NODE_STMT_DECL;
        Token Type  = *Peek(0);

        Consume(); // type
        Consume(); // colon

        b8 IsMutable = Peek(0) && Peek(0)->Subtype == _KEY_VAR;
        if (IsMutable) {
            Stmt->Holds = _NODE_STMT_MDECL;
            Consume();
        }

        // assignment, expects [id][=]
        if (Peek(0) && Peek(0)->Type == _ID && Peek(1) && Peek(1)->Subtype == _OP_EQ) {

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
    }

    else {
        fprintf(stderr, "SyntaxError: Invalid statement; got '%s'", (char*)Peek(0)->Value);
        return false;
    }

    // all statements must end in a semicolon
    if (!Peek(0) || !(Peek(0)->Subtype == _SPEC_SEMI)) {
        fprintf(
            stderr, "SyntaxError: Missing semicolon; got '%s'.\n",
            !Peek(0) ? "" : (char*)Peek(0)->Value
        );
        return false;
    }

    Consume(); // semicolon
    return true;
}

static b8 ParseRoot(NodeRoot* Tree) {
    while (Peek(0)) {

        NodeStmt Stmt = { 0 };
        if (ParseStmt(&Stmt)) {
            Tree->Stats[Tree->StatsLen++] = Stmt;

        } else {
            fprintf(stderr, "SyntaxError: Invalid statement; got '%s'.\n", (char*)Peek(0)->Value);
            return false;
        }
    }

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

    if (!InitParser(Tokens, TokensLen)) {
        fputs("ParseError: Parser initialization failed.\n", stderr);
        return false;
    }

    if (!ParseRoot(Tree)) {
        fputs("ParseError: Could not parse to exit statement.\n", stderr);
        return false;
    }

    DestroyParser();
    return true;
}
