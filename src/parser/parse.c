#include "parse.h"

#include <stdio.h>

#include "../arena.h"
#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../types.h"
#include "node_types.h"
#include "parser.h"

static b8 ParseTerm(NodeTerm* Term) {
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
        fprintf(
            stderr, "SyntaxError: Invalid term; got '%s'.\n",
            Peek(0) ? (char*)Peek(0)->Value : "null"
        );
        return false;
    }

    Consume();
    return true;
}

static b8 ParseExpr(NodeExpr* Expr) {
    Expr->Holds = _TERM;
    Expr->Term  = Alloc(NodeTerm, 1);

    if (!ParseTerm(Expr->Term)) {
        fputs("SyntaxError: Invalid term.\n", stderr);
        return false;
    }

    return true;
}

static b8 ParseStmt(NodeStmt* Stmt) {

    // exit statement, expects [exit]
    if (Peek(0) && Peek(0)->Subtype == _KEY_EXIT) {
        Consume(); // exit

        Stmt->Holds   = _STMT_EXIT;

        Stmt->Exit    = Alloc(NodeStmtExit, 1);
        *(Stmt->Exit) = (NodeStmtExit){ .Expr = Alloc(NodeExpr, 1) };

        if (!ParseExpr(Stmt->Exit->Expr)) {
            fputs("SyntaxError: Invalid expression.\n", stderr);
            return false;
        }
    }

    // assignment, expects [id][=][expr]
    else if (Peek(0) && Peek(0)->Type == _ID && Peek(1) && Peek(1)->Subtype == _OP_EQ) {

        Stmt->Holds = _STMT_ASGN;
        Token Ident = *Peek(0);

        Consume(); // id
        Consume(); // equals

        Stmt->Asgn       = Alloc(NodeStmtAsgn, 1);
        Stmt->Asgn->Expr = Alloc(NodeExpr, 1);

        if (!ParseExpr(Stmt->Asgn->Expr)) {
            fputs("SyntaxError: Invalid assignment expression.", stderr);
            return false;
        }

        Stmt->Asgn->Ident    = Alloc(NodeTermIdent, 1);
        *(Stmt->Asgn->Ident) = (NodeTermIdent){ .Id = Ident };
    }

    // definition or declaration, expects [type][:]
    else if (Peek(0) && Peek(0)->Type == _KEY && Peek(1) && Peek(1)->Subtype == _SPEC_COLON) {

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

            return true;
        }

        // definition, expects [=][expr]
        if (Peek(0) && Peek(0)->Subtype == _OP_EQ) {
            Consume(); // equals

            Stmt->Def       = Alloc(NodeStmtDef, 1);
            Stmt->Def->Expr = Alloc(NodeExpr, 1);

            if (!ParseExpr(Stmt->Def->Expr)) {
                fputs("SyntaxError: Invalid assignment expression.", stderr);
                return false;
            }

            Stmt->Def->Type  = Type;
            Stmt->Def->Ident = Alloc(NodeTermIdent, 1);
            *(Stmt->Decl->Ident) =
                (NodeTermIdent){ .Id = Ident, .IsMutable = IsMutable, .IsInit = true };
        }

        else {
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
