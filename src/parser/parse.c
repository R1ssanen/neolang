#include "parse.h"

#include <stdio.h>

#include "../arena.h"
#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../types.h"
#include "node_types.h"
#include "parser.h"

static b8 ParseExpr(NodeExpr* Expr) {
    if (Peek(0)->Type == _NUMLIT) {
        Expr->NumLit  = Alloc(NodeExprNumLit, 1);
        *Expr->NumLit = (NodeExprNumLit){ .Num = *Peek(0) };
        Expr->Holds   = _NODE_NUMLIT;
    }

    else if (Peek(0)->Type == _STRLIT) {
        Expr->StrLit  = Alloc(NodeExprStrLit, 1);
        *Expr->StrLit = (NodeExprStrLit){ .Str = *Peek(0) };
        Expr->Holds   = _NODE_STRLIT;
    }

    else if (Peek(0)->Type == _ID) {
        Expr->Ident  = Alloc(NodeExprId, 1);
        *Expr->Ident = (NodeExprId){ .Id = *Peek(0) };
        Expr->Holds  = _NODE_ID;
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

        Stmt->Holds   = _NODE_STMT_EXIT;

        Stmt->Exit    = Alloc(NodeStmtExit, 1);
        *(Stmt->Exit) = (NodeStmtExit){ .Expr = Alloc(NodeExpr, 1) };

        if (!ParseExpr(Stmt->Exit->Expr)) {
            fputs("SyntaxError: Invalid expression.\n", stderr);
            return false;
        }
    }

    // declaration, expects [type][:][id]
    else if (Peek(0) && Peek(0)->Type == _KEY && Peek(1) && Peek(1)->Subtype == _SPEC_COLON &&
             Peek(2) && Peek(2)->Type == _ID) {

        Stmt->Holds       = _NODE_STMT_DECL;
        TokenSubtype Type = Peek(0)->Subtype;

        Consume(); // type
        Consume(); // colon

        b8 IsMutable = Peek(0) && Peek(0)->Subtype == _KEY_VAR;
        if (IsMutable) {
            Consume(); // var
        }

        Token Ident = *Peek(0);
        Consume(); // id

        // uninitialized variable
        if (Peek(0) && Peek(0)->Subtype == _SPEC_SEMI) {
            Consume(); // semi
            return true;
        }

        // assignment, expects [=][expr]
        if (Peek(0) && Peek(0)->Subtype == _OP_EQ) {
            Consume(); // equals

            Stmt->Decl       = Alloc(NodeStmtDecl, 1);
            Stmt->Decl->Expr = Alloc(NodeExpr, 1);

            if (!ParseExpr(Stmt->Decl->Expr)) {
                fputs("SyntaxError: Invalid assignment expression.", stderr);
                return false;
            }

            Stmt->Decl->Type     = Type;
            Stmt->Decl->Ident    = Alloc(NodeExprId, 1);
            *(Stmt->Decl->Ident) = (NodeExprId){ .Id = Ident, .IsMutable = IsMutable };
        }

        // uninitialized variable
        else if (Peek(0) && Peek(0)->Subtype == _SPEC_SEMI) {
            printf("ParserInfo: Uninitialized variable '%s'.\n", (char*)Ident.Value);
            return true;
        }

        else {
            fprintf(stderr, "SyntaxError: Invalid declaration; got '%s'.\n", (char*)Peek(0)->Value);
            return false;
        }
    }

    else {
        fprintf(stderr, "SyntaxError: Invalid statement; got '%s'.\n", (char*)Peek(0)->Value);
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

        NodeStmt* Stmt = Alloc(NodeStmt, 1);
        if (ParseStmt(Stmt)) {
            Tree->Stats[Tree->StatsLen++] = Stmt;
        } else {
            fputs("SyntaxError: Invalid statement.\n", stderr);
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
