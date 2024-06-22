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

    _STMT         = 0x00001000,
    _STMT_EXIT    = 0x00002000,
    _STMT_DECL    = 0x00004000,
    _STMT_ASGN    = 0x00008000,
    _STMT_DEF     = 0x00010000
} NodeType;

struct NodeTermNumLit;
struct NodeTermIdent;
struct NodeTerm;

struct NodeExpr;

struct NodeStmtExit;
struct NodeStmtDecl;
struct NodeStmtAsgn;
struct NodeStmtDef;

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

typedef struct NodeExpr {
    union {
        NodeTerm* Term;
    };

    NodeType Holds;
} NodeExpr;

typedef struct NodeStmtExit {
    NodeExpr* Expr;
} NodeStmtExit;

typedef struct NodeStmtDecl {
    NodeTermIdent* Ident;
    TokenSubtype   Type;
} NodeStmtDecl;

typedef struct NodeStmtAsgn {
    NodeTermIdent* Ident;
    NodeExpr*      Expr;
} NodeStmtAsgn;

typedef struct NodeStmtDef {
    NodeTermIdent* Ident;
    NodeExpr*      Expr;
    TokenSubtype   Type;
} NodeStmtDef;

typedef struct NodeStmt {
    union {
        NodeStmtExit* Exit;
        NodeStmtDecl* Decl;
        NodeStmtAsgn* Asgn;
        NodeStmtDef*  Def;
    };

    NodeType Holds;
} NodeStmt;

typedef struct NodeRoot {
    NodeStmt** Stats;
    u64        StatsLen;
} NodeRoot;

#endif
