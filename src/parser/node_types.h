/**
 * @file parser/node_types.h
 * @brief Abstract syntax tree nodes.
 * @author github.com/r1ssanen
 */

#ifndef NODE_TYPES_H
#define NODE_TYPES_H

#include "../lexer/token_types.h"
#include "../types.h"

typedef enum NodeType {
    _NODE_INVALID = 0,

    _TERM         = 0x00000001,
    _TERM_NUMLIT  = 0x00000002,
    _TERM_IDENT   = 0x00000004,

    _EXPR         = 0x00000100,
    _BIN_EXPR     = 0x00000200,

    _DECL         = 0x00004000,
    _ASGN         = 0x00008000,
    _VAR_DEF      = 0x00010000,
    _SCOPE        = 0x00020000,
    _INTERVAL     = 0x00040000,

    _FUN_DECL     = 0x00100000,
    _FUN_DEF      = 0x00200000,
    _CALL         = 0x00400000,

    _STMT         = 0x01000000,
    _STMT_FOR     = 0x02000000,
    _STMT_IF      = 0x04000000,
    _STMT_EXIT    = 0x08000000,
    _STMT_WHILE   = 0x10000000,
} NodeType;

struct NodeTermNumLit;
struct NodeTermIdent;
struct NodeTerm;

struct NodeBinExpr;
struct NodeExpr;

struct NodeRoot;
struct NodeDecl;
struct NodeAsgn;
struct NodeVarDef;
struct NodeScope;
struct NodeInterval;

struct NodeFunDecl;
struct NodeFunDef;
struct NodeCall;

struct NodeStmtExit;
struct NodeStmtFor;
struct NodeStmtIf;
struct NodeStmtWhile;
struct NodeStmt;

typedef struct NodeTermNumLit {
    Token Num;
} NodeTermNumLit;

typedef struct NodeTermIdent {
    Token Id;
    b8    IsMutable;
    b8    IsInit;
} NodeTermIdent;

typedef struct NodeTerm {
    union {
        struct NodeTermNumLit* NumLit;
        struct NodeTermIdent*  Ident;
    };

    NodeType Holds;
} NodeTerm;

typedef struct NodeBinExpr {
    struct NodeExpr* LHS;
    struct NodeExpr* RHS;
    TokenSubtype     Op;
} NodeBinExpr;

typedef struct NodeExpr {
    union {
        struct NodeTerm*     Term;
        struct NodeBinExpr*  BinExpr;
        struct NodeInterval* Interval;
        struct NodeAsgn*     Asgn;
        struct NodeCall*     Call;
    };

    NodeType Holds;
} NodeExpr;

typedef struct NodeDecl {
    struct NodeTermIdent* Ident;
    TokenSubtype          Type;
} NodeDecl;

typedef struct NodeAsgn {
    struct NodeTermIdent* Ident;
    struct NodeExpr*      Expr;
} NodeAsgn;

typedef struct NodeVarDef {
    struct NodeTermIdent* Ident;
    struct NodeExpr*      Expr;
    TokenSubtype          Type;
} NodeVarDef;

typedef struct NodeScope {
    struct NodeStmt** Stats;
    u64               StatCount;
    u32               ScopeID;
} NodeScope;

typedef struct NodeInterval {
    struct NodeExpr* Beg;
    struct NodeExpr* End;
} NodeInterval;

typedef struct NodeCall {
    struct NodeExpr**     ArgExprs;
    struct NodeTermIdent* Ident;
    u64                   ArgCount;
} NodeCall;

typedef struct NodeFunDecl {
    struct NodeDecl** ArgDecls;
    struct NodeDecl*  Decl;
    u64               ArgCount;
} NodeFunDecl;

typedef struct NodeFunDef {
    struct NodeFunDecl* Decl;
    struct NodeScope*   Scope;
} NodeFunDef;

typedef struct NodeStmtExit {
    struct NodeExpr* Expr;
} NodeStmtExit;

typedef struct NodeStmtFor {
    struct NodeVarDef* Def;
    struct NodeScope*  Scope;
    struct NodeExpr*   Expr;
    TokenSubtype       Type;
} NodeStmtFor;

typedef struct NodeStmtIf {
    struct NodeScope* Scope;
    struct NodeExpr*  Expr;
} NodeStmtIf;

typedef struct NodeStmtWhile {
    struct NodeScope* Scope;
    struct NodeExpr*  Expr;
} NodeStmtWhile;

typedef struct NodeStmt {
    union {
        struct NodeDecl*      Decl;
        struct NodeVarDef*    VarDef;
        struct NodeAsgn*      Asgn;
        struct NodeScope*     Scope;
        struct NodeFunDecl*   FunDecl;
        struct NodeFunDef*    FunDef;
        struct NodeCall*      Call;
        struct NodeStmtExit*  Exit;
        struct NodeStmtFor*   For;
        struct NodeStmtIf*    If;
        struct NodeStmtWhile* While;
    };

    NodeType Holds;
} NodeStmt;

typedef struct NodeRoot {
    struct NodeStmt** Stats;
    u64               StatCount;
} NodeRoot;

#endif
