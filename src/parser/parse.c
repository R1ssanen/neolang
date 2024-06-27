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

NodeTermNumLit* TryParseNumLit() {
    Token* Num = TryConsumeType(_NUMLIT);
    if (!Num) { return NULL; }

    NodeTermNumLit* NumLit = Alloc(NodeTermNumLit, 1);
    NumLit->Num            = *Num;

    return NumLit;
}

NodeTermIdent* TryParseIdent() {
    Token* Id = TryConsumeType(_ID);
    if (!Id) { return NULL; }

    NodeTermIdent* Ident = Alloc(NodeTermIdent, 1);
    Ident->Id            = *Id;

    return Ident;
}

NodeTerm* TryParseTerm() {
    NodeTermNumLit* NumLit = TryParseNumLit();
    if (NumLit) {
        NodeTerm* Term = Alloc(NodeTerm, 1);
        Term->Holds    = _TERM_NUMLIT;
        Term->NumLit   = NumLit;
        return Term;
    }

    NodeTermIdent* Ident = TryParseIdent();
    if (Ident) {
        NodeTerm* Term = Alloc(NodeTerm, 1);
        Term->Holds    = _TERM_IDENT;
        Term->Ident    = Ident;
        return Term;
    }

    else {
        return NULL;
    }
}

NodeExpr* TryParseExpr(u8 MinPrec) {
    NodeExpr* LHS = NULL;

    // subexpr
    if (TryConsumeSub(_SPEC_LPAREN)) {
        if (!(LHS = TryParseExpr(0))) {
            THROW_ERROR(_SYNTAX_ERROR, "Could not parse subexpression.");
            return NULL;
        }

        if (!TryConsumeSub(_SPEC_RPAREN)) {
            THROW_ERROR(_SYNTAX_ERROR, "Unmatched parentheses.");
            return NULL;
        }
    }

    else {
        LHS        = Alloc(NodeExpr, 1);
        LHS->Holds = _TERM;
        LHS->Term  = TryParseTerm();
        if (!LHS->Term) {
            THROW_ERROR(_SYNTAX_ERROR, "Could not parse term for expression.");
            return NULL;
        }
    }

    while (Peek(0)) {
        //  FIX: not every operator is a binary operator
        if (Peek(0)->Type != _OP || OPINFOS[Peek(0)->Subtype].Prec < MinPrec) { break; }

        Token*    Op          = Consume();
        OpInfo    Info        = OPINFOS[Op->Subtype];
        u8        NextMinPrec = (Info.Assoc == _ASSOC_LEFT) ? Info.Prec + 1 : Info.Prec;

        NodeExpr* RHS         = TryParseExpr(NextMinPrec);
        if (!RHS) {
            THROW_ERROR(_SYNTAX_ERROR, "Could not parse bin_expr.");
            return NULL;
        }

        NodeExpr* NewLHS     = Alloc(NodeExpr, 1);
        NewLHS->Holds        = _BIN_EXPR;
        NewLHS->BinExpr      = Alloc(NodeBinExpr, 1);
        NewLHS->BinExpr->RHS = RHS;
        NewLHS->BinExpr->Op  = Op->Subtype;

        NewLHS->BinExpr->LHS = Alloc(NodeExpr, 1);
        memcpy(NewLHS->BinExpr->LHS, LHS, sizeof(NodeExpr));
        memcpy(LHS, NewLHS, sizeof(NodeExpr));
    }

    return LHS;
}

NodeInterval* TryParseInterval() {
    if (!TryConsumeSub(_SPEC_LBRACK)) { return NULL; }

    NodeExpr* Beg = NULL;
    if (!(Beg = TryParseExpr(0))) { return NULL; }

    if (!TryConsumeSub(_SPEC_PERIOD)) { return NULL; }
    if (!TryConsumeSub(_SPEC_PERIOD)) { return NULL; }

    NodeExpr* End = NULL;
    if (!(End = TryParseExpr(0))) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse expression for interval.");
        return NULL;
    }

    if (!TryConsumeSub(_SPEC_RBRACK)) {
        THROW_ERROR(_SYNTAX_ERROR, "Unclosed interval.");
        return NULL;
    }

    NodeInterval* Interval = Alloc(NodeInterval, 1);
    Interval->Beg          = Beg;
    Interval->End          = End;

    return Interval;
}

NodeStmtAsgn* TryParseAsgn() {
    Token* Ident = TryConsumeType(_ID);
    if (!Ident) { return NULL; }
    if (!TryConsumeSub(_OP_EQ)) { return NULL; }

    NodeExpr* Expr = NULL;
    if (!(Expr = TryParseExpr(0))) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse expr for assign.");
        return NULL;
    }

    NodeStmtAsgn* Asgn = Alloc(NodeStmtAsgn, 1);
    Asgn->Expr         = Expr;
    Asgn->Ident        = Alloc(NodeTermIdent, 1);
    *(Asgn->Ident)     = (NodeTermIdent){ .Id = *Ident };

    return Asgn;
}

NodeVarDef* TryParseVarDef() {
    NodeDecl* Decl = NULL;
    if (!(Decl = TryParseDecl())) { return NULL; }
    if (!TryConsumeSub(_OP_EQ)) { return NULL; }

    NodeExpr* Expr = NULL;
    if (!(Expr = TryParseExpr(0))) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse expr for var_def.");
        return NULL;
    }

    NodeVarDef* Def = Alloc(NodeVarDef, 1);
    Def->Type       = Decl->Type;
    Def->Ident      = Decl->Ident;
    Def->Expr       = Expr;

    return Def;
}

NodeDecl* TryParseDecl() {
    Token* Type = TryConsumeType(_BITYPE);
    if (!Type) { return NULL; }
    if (!TryConsumeSub(_SPEC_COLON)) { return NULL; }

    b8     IsMutable = TryConsumeSub(_KEY_VAR) ? true : false;

    Token* Ident     = TryConsumeType(_ID);
    if (!Ident) { return NULL; }

    NodeDecl* Decl = Alloc(NodeDecl, 1);
    Decl->Type     = Type->Subtype;
    Decl->Ident    = Alloc(NodeTermIdent, 1);
    *(Decl->Ident) = (NodeTermIdent){ .Id = *Ident, .IsMutable = IsMutable, .IsInit = false };

    return Decl;
}

NodeScope* TryParseScope() {
    if (!TryConsumeSub(_SPEC_LBRACE)) { return NULL; }

    const u64  MAX_SCOPE_STATS = 1000;
    NodeScope* Scope           = Alloc(NodeScope, 1);
    Scope->Stats               = Alloc(NodeStmt*, MAX_SCOPE_STATS);

    while (Peek(0) && Peek(0)->Subtype != _SPEC_RBRACE) {
        NodeStmt* Stmt = NULL;

        if (!(Stmt = TryParseStmt())) {
            THROW_ERROR(_SYNTAX_ERROR, "Could not parse statement for scope.");
            return NULL;
        }

        Scope->Stats[Scope->StatsLen++] = Stmt;
    }

    if (!TryConsumeSub(_SPEC_RBRACE)) {
        THROW_ERROR(_SYNTAX_ERROR, "Unclosed scope.");
        return NULL;
    }

    return Scope;
}

NodeStmtFor* TryParseFor() {
    if (!TryConsumeSub(_KEY_FOR)) { return NULL; }

    NodeVarDef* Def = NULL;
    if (!(Def = TryParseVarDef())) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse definition for loop iterator.");
        return NULL;
    }

    if (!TryConsumeSub(_SPEC_SEMI)) {
        THROW_ERROR(_SYNTAX_ERROR, "Missing semicolon.");
        return NULL;
    }

    NodeInterval* Interval = NULL;
    if (!(Interval = TryParseInterval())) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse interval for loop.");
        return NULL;
    }

    NodeScope* Scope = NULL;
    if (!(Scope = TryParseScope())) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse scope for loop.");
        return NULL;
    }

    NodeStmtFor* For = Alloc(NodeStmtFor, 1);
    For->Def         = Def;
    For->Type        = Def->Type;
    For->Scope       = Scope;
    For->Interval    = Interval;

    return For;
}

NodeExit* TryParseExit() {
    if (!TryConsumeSub(_KEY_EXIT)) { return NULL; }

    NodeExpr* Expr = NULL;
    if (!(Expr = TryParseExpr(0))) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse expression for exit.");
        return NULL;
    }

    NodeExit* Exit = Alloc(NodeExit, 1);
    Exit->Expr     = Expr;

    return Exit;
}

NodeStmt* TryParseStmt() {
    NodeStmt* Stmt = Alloc(NodeStmt, 1);

    NodeExit* Exit = TryParseExit();
    if (Exit) {
        Stmt->Holds = _EXIT;
        Stmt->Exit  = Exit;

        if (!TryConsumeSub(_SPEC_SEMI)) {
            THROW_ERROR(_SYNTAX_ERROR, "Missing semicolon.");
            return NULL;
        }

        return Stmt;
    }

    NodeStmtFor* For = TryParseFor();
    if (For) {
        Stmt->Holds = _STMT_FOR;
        Stmt->For   = For;

        return Stmt;
    }

    NodeScope* Scope = TryParseScope();
    if (Scope) {
        Stmt->Holds = _SCOPE;
        Stmt->Scope = Scope;

        return Stmt;
    }

    NodeStmtAsgn* Asgn = TryParseAsgn();
    if (Asgn) {
        Stmt->Holds = _STMT_ASGN;
        Stmt->Asgn  = Asgn;

        if (!TryConsumeSub(_SPEC_SEMI)) {
            THROW_ERROR(_SYNTAX_ERROR, "Missing semicolon.");
            return NULL;
        }

        return Stmt;
    }

    NodeVarDef* Def = TryParseVarDef();
    if (Def) {
        Stmt->Holds  = _VAR_DEF;
        Stmt->VarDef = Def;

        if (!TryConsumeSub(_SPEC_SEMI)) {
            THROW_ERROR(_SYNTAX_ERROR, "Missing semicolon.");
            return NULL;
        }

        return Stmt;
    }

    NodeDecl* Decl = TryParseDecl();
    if (Decl) {
        Stmt->Holds = _DECL;
        Stmt->Decl  = Decl;

        if (!TryConsumeSub(_SPEC_SEMI)) {
            THROW_ERROR(_SYNTAX_ERROR, "Missing semicolon.");
            return NULL;
        }

        return Stmt;
    }

    THROW_ERROR(
        _SYNTAX_ERROR, "Invalid statement; got '%s'.",
        Peek(0) ? GetTypeDebugName(Peek(0)->Type) : "null"
    );
    return NULL;
}

NodeRoot* TryParseRoot() {

    const u64 MAX_NODE_STATS = 1000;
    NodeRoot* Tree           = Alloc(NodeRoot, 1);
    Tree->Stats              = Alloc(NodeStmt*, MAX_NODE_STATS);

    while (Peek(0)) {
        NodeStmt* Stmt = TryParseStmt();
        if (!Stmt) { return NULL; }

        Tree->Stats[Tree->StatsLen++] = Stmt;
    }

    return Tree;
}

NodeRoot* Parse(const Token* Tokens, u64 TokensLen) {
    if (!Tokens) {
        THROW_ERROR(_INVALID_ARG, "No tokens to parse.");
        return NULL;
    }

    if (!InitParser(Tokens, TokensLen)) { return NULL; }

    NodeRoot* Tree = TryParseRoot();
    if (!Tree) {
        THROW_ERROR(_SYNTAX_ERROR, "Could not parse program root.");
        return NULL;
    }

    return Tree;
}
