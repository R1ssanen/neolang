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

    _EXIT         = 0x00002000,
    _DECL         = 0x00004000,
    _VAR_DEF      = 0x00008000,
    _SCOPE        = 0x00010000,
    _INTERVAL     = 0x00020000,

    _STMT         = 0x00100000,
    _STMT_ASGN    = 0x00200000,
    _STMT_FOR     = 0x00400000,
    _STMT_IF      = 0x00800000,
} NodeType;

struct NodeTermNumLit;
struct NodeTermIdent;
struct NodeTerm;

struct NodeRoot;
struct NodeBinExpr;
struct NodeExpr;
struct NodeExit;
struct NodeDecl;
struct NodeVarDef;
struct NodeScope;
struct NodeInterval;

// struct NodeFunDecl;
// struct NodeFunDef;

struct NodeStmtAsgn;
struct NodeStmtFor;
struct NodeStmtIf;
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
        NodeTermNumLit* NumLit;
        NodeTermIdent*  Ident;
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
        NodeTerm*    Term;
        NodeBinExpr* BinExpr;
    };

    NodeType Holds;
} NodeExpr;

typedef struct NodeExit {
    NodeExpr* Expr;
} NodeExit;

typedef struct NodeDecl {
    NodeTermIdent* Ident;
    TokenSubtype   Type;
} NodeDecl;

typedef struct NodeVarDef {
    NodeTermIdent* Ident;
    NodeExpr*      Expr;
    TokenSubtype   Type;
} NodeVarDef;

typedef struct NodeScope {
    struct NodeStmt** Stats;
    u64               StatsLen;
    u32               ScopeID;
} NodeScope;

typedef struct NodeInterval {
    NodeExpr* Beg;
    NodeExpr* End;
} NodeInterval;

typedef struct NodeStmtAsgn {
    NodeTermIdent* Ident;
    NodeExpr*      Expr;
} NodeStmtAsgn;

typedef struct NodeStmtFor {
    NodeVarDef*   Def;
    NodeScope*    Scope;
    NodeInterval* Interval;
    TokenSubtype  Type;
} NodeStmtFor;

typedef struct NodeStmtIf {
    NodeScope* Scope;
    NodeExpr*  Expr;
} NodeStmtIf;

typedef struct NodeStmt {
    union {
        NodeExit*     Exit;
        NodeDecl*     Decl;
        NodeVarDef*   VarDef;
        NodeStmtAsgn* Asgn;
        NodeScope*    Scope;
        NodeStmtFor*  For;
        NodeStmtIf*   If;
    };

    NodeType Holds;
} NodeStmt;

typedef struct NodeRoot {
    NodeStmt** Stats;
    u64        StatsLen;
} NodeRoot;

#endif
