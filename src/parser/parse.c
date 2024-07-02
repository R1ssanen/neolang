#include "parse.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/opinfo.h"
#include "../lexer/token_subtypes.h"
#include "../lexer/token_types.h"
#include "../limits.h"
#include "../types.h"
#include "../util/arena.h"
#include "../util/error.h"
#include "node_types.h"
#include "parser.h"

NodeStmtWhile* TryParseWhile(void) {
    if (!TryConsumeSub(_KEY_WHILE)) { return NULL; }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse expr for while-statement.");
        return NULL;
    }

    NodeScope* Scope = TryParseScope();
    if (!Scope) {
        SYNTAX_ERR("Could not parse scope for while-statement.");
        return NULL;
    }

    NodeStmtWhile* While = Alloc(NodeStmtWhile, 1);
    While->Scope         = Scope;
    While->Expr          = Expr;

    return While;
}

NodeFunDef* TryParseFunDef(void) {
    NodeFunDecl* Decl = TryParseFunDecl();
    if (!Decl) { return NULL; }

    NodeScope* Scope = TryParseScope();
    if (!Scope) {
        SYNTAX_ERR("Could not parse scope for fun-def.");
        return NULL;
    }

    NodeFunDef* FunDef = Alloc(NodeFunDef, 1);
    FunDef->Decl       = Decl;
    FunDef->Scope      = Scope;

    return FunDef;
}

NodeFunDecl* TryParseFunDecl(void) {
    NodeDecl* IdentDecl = TryParseDecl();
    if (!IdentDecl) { return NULL; }
    if (!TryConsumeSub(_SPEC_LPAREN)) { return NULL; }

    NodeFunDecl* FunDecl = Alloc(NodeFunDecl, 1);

    FunDecl->ArgDecls    = Alloc(NodeDecl*, MAX_FUN_ARGS);
    FunDecl->ArgCount    = 0;
    FunDecl->Decl        = IdentDecl;

    // arguments comma separated
    while (!TryConsumeSub(_SPEC_RPAREN)) {
        NodeDecl* Decl = TryParseDecl();
        if (!Decl) {
            SYNTAX_ERR("Could not parse decl for call.");
            return NULL;
        }

        FunDecl->ArgDecls[FunDecl->ArgCount++] = Decl;

        if (!TryConsumeSub(_SPEC_COMMA) && Peek(1)->Subtype != _SPEC_RPAREN) {
            SYNTAX_ERR("Function arguments must be comma separated.");
            return NULL;
        }
    }

    return FunDecl;
}

NodeCall* TryParseCall(void) {
    NodeTermIdent* Ident = TryParseIdent();
    if (!Ident) { return NULL; }
    if (!TryConsumeSub(_SPEC_LPAREN)) { return NULL; }

    NodeCall* Call = Alloc(NodeCall, 1);

    Call->ArgExprs = Alloc(NodeExpr*, MAX_FUN_ARGS);
    Call->ArgCount = 0;
    Call->Ident    = Ident;

    // arguments comma separated
    while (!TryConsumeSub(_SPEC_RPAREN)) {
        NodeExpr* Expr = TryParseExpr();
        if (!Expr) {
            SYNTAX_ERR("Could not parse argument for call.");
            return NULL;
        }

        Call->ArgExprs[Call->ArgCount++] = Expr;

        if (!TryConsumeSub(_SPEC_COMMA) && Peek(1)->Subtype != _SPEC_RPAREN) {
            SYNTAX_ERR("Function arguments must be comma separated.");
            return NULL;
        }
    }

    return Call;
}

NodeTermNumLit* TryParseNumLit(void) {
    Token* Num = TryConsumeType(_NUMLIT);
    if (!Num) { return NULL; }

    NodeTermNumLit* NumLit = Alloc(NodeTermNumLit, 1);
    NumLit->Num            = *Num;

    return NumLit;
}

NodeTermIdent* TryParseIdent(void) {
    Token* Id = TryConsumeType(_ID);
    if (!Id) { return NULL; }

    NodeTermIdent* Ident = Alloc(NodeTermIdent, 1);
    Ident->Id            = *Id;

    return Ident;
}

NodeTerm* TryParseTerm(void) {
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

NodeExpr* TryParseBinExpr(u8 MinPrec) {
    NodeExpr* Expr = Alloc(NodeExpr, 1);

    // subexpr
    if (TryConsumeSub(_SPEC_LPAREN)) {
        if (!(Expr = TryParseBinExpr(0))) {
            SYNTAX_ERR("Could not parse subexpression.");
            return NULL;
        }

        if (!TryConsumeSub(_SPEC_RPAREN)) {
            SYNTAX_ERR("Unmatched parentheses.");
            return NULL;
        }
    }

    else {
        NodeTerm* Term = TryParseTerm();
        if (!Term) {
            SYNTAX_ERR("Could not parse term for expression.");
            return NULL;
        }

        Expr->Holds = _TERM;
        Expr->Term  = Term;
    }

    while (Peek(0)) {
        // Token* Op = TryConsumeType(_OP);
        //    FIX: not every operator is a binary operator
        // if (!Op || OPINFOS[Op->Subtype].Prec < MinPrec) { break; }
        if (Peek(0)->Type != _OP || OPINFOS[Peek(0)->Subtype].Prec < MinPrec) { break; }
        Token*    Op          = Consume();

        OpInfo    Info        = OPINFOS[Op->Subtype];
        u8        NextMinPrec = (Info.Assoc == _ASSOC_LEFT) ? Info.Prec + 1 : Info.Prec;

        NodeExpr* RHS         = TryParseBinExpr(NextMinPrec);
        if (!RHS) {
            SYNTAX_ERR("Could not parse bin_expr.");
            return NULL;
        }

        NodeExpr* LHS = Alloc(NodeExpr, 1);
        memcpy(LHS, Expr, sizeof(NodeExpr));

        Expr->Holds        = _BIN_EXPR;
        Expr->BinExpr      = Alloc(NodeBinExpr, 1);
        Expr->BinExpr->Op  = Op->Subtype;
        Expr->BinExpr->LHS = LHS;
        Expr->BinExpr->RHS = RHS;
    }

    return Expr;
}

NodeExpr* TryParseExpr() {
    NodeExpr*     Expr     = NULL;

    NodeInterval* Interval = TryParseInterval();
    if (Interval) {
        Expr           = Alloc(NodeExpr, 1);
        Expr->Holds    = _INTERVAL;
        Expr->Interval = Interval;
        return Expr;
    }

    NodeAsgn* Asgn = TryParseAsgn();
    if (Asgn) {
        Expr        = Alloc(NodeExpr, 1);
        Expr->Holds = _ASGN;
        Expr->Asgn  = Asgn;
        return Expr;
    }

    NodeCall* Call = TryParseCall();
    if (Call) {
        Expr        = Alloc(NodeExpr, 1);
        Expr->Holds = _CALL;
        Expr->Call  = Call;
        return Expr;
    }

    if ((Expr = TryParseBinExpr(0))) { return Expr; }

    SYNTAX_ERR("Invalid expression. [%lu, %lu]", Peek(0)->Line, Peek(0)->Column);
    return NULL;
}

NodeInterval* TryParseInterval(void) {
    if (!TryConsumeSub(_SPEC_LBRACK)) { return NULL; }

    NodeExpr* Beg = TryParseExpr();
    if (!Beg) { return NULL; }

    if (!TryConsumeSub(_SPEC_COMMA)) { return NULL; }

    NodeExpr* End = TryParseExpr();
    if (!End) {
        SYNTAX_ERR("Could not parse expression for interval.");
        return NULL;
    }

    if (!TryConsumeSub(_SPEC_RBRACK)) {
        SYNTAX_ERR("Unclosed interval.");
        return NULL;
    }

    NodeInterval* Interval = Alloc(NodeInterval, 1);
    Interval->Beg          = Beg;
    Interval->End          = End;

    return Interval;
}

NodeAsgn* TryParseAsgn(void) {
    NodeTermIdent* Ident = TryParseIdent();
    if (!Ident) { return NULL; }
    if (!TryConsumeSub(_OP_EQ)) { return NULL; }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse expr for assign.");
        return NULL;
    }

    NodeAsgn* Asgn = Alloc(NodeAsgn, 1);
    Asgn->Expr     = Expr;
    Asgn->Ident    = Ident;

    return Asgn;
}

NodeVarDef* TryParseVarDef(void) {
    NodeDecl* Decl = TryParseDecl();
    if (!Decl) { return NULL; }
    if (!TryConsumeSub(_OP_EQ)) { return NULL; }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse expr for var_def.");
        return NULL;
    }

    NodeVarDef* Def    = Alloc(NodeVarDef, 1);
    Def->Expr          = Expr;
    Def->Type          = Decl->Type;
    Def->Ident         = Decl->Ident;
    Def->Ident->IsInit = true;

    return Def;
}

NodeDecl* TryParseDecl(void) {
    Token* Type = TryConsumeType(_BITYPE);
    if (!Type) { return NULL; }
    if (!TryConsumeSub(_SPEC_COLON)) { return NULL; }

    b8             IsMutable = TryConsumeSub(_KEY_VAR) ? true : false;

    NodeTermIdent* Ident     = TryParseIdent();
    if (!Ident) { return NULL; }

    NodeDecl* Decl         = Alloc(NodeDecl, 1);
    Decl->Type             = Type->Subtype;
    Decl->Ident            = Ident;
    Decl->Ident->IsMutable = IsMutable;
    Decl->Ident->IsInit    = false;

    return Decl;
}

NodeScope* TryParseScope(void) {
    if (!TryConsumeSub(_SPEC_LBRACE)) { return NULL; }

    NodeStmt*  Stats[MAX_SCOPE_STATS];

    NodeScope* Scope = Alloc(NodeScope, 1);
    Scope->StatCount = 0;

    while (Peek(0) && Peek(0)->Subtype != _SPEC_RBRACE) {
        NodeStmt* Stmt = TryParseStmt();

        if (!Stmt) {
            // SYNTAX_ERR("Could not parse statement for scope.");
            // return NULL;
            continue;
        }

        Stats[Scope->StatCount++] = Stmt;
    }

    if (!TryConsumeSub(_SPEC_RBRACE)) {
        SYNTAX_ERR("Unclosed scope.");
        return NULL;
    }

    Scope->Stats = Alloc(NodeStmt*, Scope->StatCount);
    memcpy(Scope->Stats, Stats, Scope->StatCount * sizeof(NodeStmt*));
    return Scope;
}

NodeStmtIf* TryParseIf(void) {
    if (!TryConsumeSub(_KEY_IF)) { return NULL; }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse expr for if-statement.");
        return NULL;
    }

    NodeScope* Scope = TryParseScope();
    if (!Scope) {
        SYNTAX_ERR("Could not parse scope for if-statement.");
        return NULL;
    }

    NodeStmtIf* If = Alloc(NodeStmtIf, 1);
    If->Scope      = Scope;
    If->Expr       = Expr;

    return If;
}

NodeStmtFor* TryParseFor(void) {
    if (!TryConsumeSub(_KEY_FOR)) { return NULL; }

    NodeVarDef* Def = TryParseVarDef();
    if (!Def) {
        SYNTAX_ERR("Could not parse definition for loop iterator.");
        return NULL;
    }

    if (!TryConsumeSub(_SPEC_COLON)) {
        SYNTAX_ERR("Missing semicolon in for-statement.");
        return NULL;
    }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse range expr for for-statement.");
        return NULL;
    }

    NodeScope* Scope = TryParseScope();
    if (!Scope) {
        SYNTAX_ERR("Could not parse scope for for-statement.");
        return NULL;
    }

    NodeStmtFor* For = Alloc(NodeStmtFor, 1);
    For->Def         = Def;
    For->Type        = Def->Type;
    For->Scope       = Scope;
    For->Expr        = Expr;

    return For;
}

NodeStmtExit* TryParseExit(void) {
    if (!TryConsumeSub(_KEY_EXIT)) { return NULL; }

    NodeExpr* Expr = TryParseExpr();
    if (!Expr) {
        SYNTAX_ERR("Could not parse expression for exit.");
        return NULL;
    }

    NodeStmtExit* Exit = Alloc(NodeStmtExit, 1);
    Exit->Expr         = Expr;

    return Exit;
}

NodeStmt* TryParseStmt(void) {
    NodeStmt* Stmt = Alloc(NodeStmt, 1);

    if ((Stmt->For = TryParseFor())) {
        Stmt->Holds = _STMT_FOR;
        return Stmt;
    }

    else if ((Stmt->If = TryParseIf())) {
        Stmt->Holds = _STMT_IF;
        return Stmt;
    }

    else if ((Stmt->While = TryParseWhile())) {
        Stmt->Holds = _STMT_WHILE;
        return Stmt;
    }

    else if ((Stmt->Scope = TryParseScope())) {
        Stmt->Holds = _SCOPE;
        return Stmt;
    }

    else if ((Stmt->Exit = TryParseExit())) {
        Stmt->Holds = _STMT_EXIT;
    }

    else if ((Stmt->Asgn = TryParseAsgn())) {
        Stmt->Holds = _ASGN;
    }

    else if ((Stmt->VarDef = TryParseVarDef())) {
        Stmt->Holds = _VAR_DEF;
    }

    else if ((Stmt->Decl = TryParseDecl())) {
        Stmt->Holds = _DECL;
    }

    else if ((Stmt->FunDef = TryParseFunDef())) {
        Stmt->Holds = _FUN_DEF;
    }

    else if ((Stmt->FunDecl = TryParseFunDecl())) {
        Stmt->Holds = _FUN_DECL;
    }

    else if ((Stmt->Call = TryParseCall())) {
        Stmt->Holds = _CALL;
    }

    else {
        printf("%s, [%lu, %lu]\n", (char*)Peek(0)->Value, Peek(0)->Line, Peek(0)->Column);
        return NULL;
    }

    if (!TryConsumeSub(_SPEC_SEMI)) {
        SYNTAX_ERR("Missing semicolon. [%lu, %lu]", Peek(0)->Line, Peek(0)->Column);
    }

    return Stmt;
}

NodeRoot* TryParseRoot(void) {
    NodeRoot* Tree  = Alloc(NodeRoot, 1);
    Tree->Stats     = Alloc(NodeStmt*, MAX_NODE_STATS);
    Tree->StatCount = 0;

    b8 HasErrors    = false;
    while (Peek(0)) {
        assert(Tree->StatCount < MAX_NODE_STATS && "Max statement count exceeded.");

        NodeStmt* Stmt = TryParseStmt();
        if (!Stmt) {
            HasErrors = true;
        } else {
            Tree->Stats[Tree->StatCount++] = Stmt;
        }
    }

    if (HasErrors) { return NULL; }
    return Tree;
}

NodeRoot* Parse(const Token* Tokens, u64 TokenCount) {
    if (!Tokens) { ARG_ERR("No tokens to parse."); }

    InitParser(Tokens, TokenCount);
    return TryParseRoot();
}
