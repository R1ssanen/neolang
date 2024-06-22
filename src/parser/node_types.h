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
    _NODE_INVALID   = 0,

    _NODE_NUMLIT    = 0x00000001,
    _NODE_STRLIT    = 0x00000002,
    _NODE_ID        = 0x00000004,
    _NODE_EXPR      = 0x00000008,
    _NODE_STMT      = 0x00000010,
    _NODE_STMT_EXIT = 0x00000020,
    _NODE_STMT_DECL = 0x00000040,
} NodeType;

typedef struct NodeExprNumLit {
    Token Num;
} NodeExprNumLit;

typedef struct NodeExprStrLit {
    Token Str;
} NodeExprStrLit;

typedef struct NodeExprId {
    Token Id;
    b8    IsMutable;
} NodeExprId;

typedef struct NodeExpr {
    union {
        NodeExprNumLit* NumLit;
        NodeExprStrLit* StrLit;
        NodeExprId*     Ident;
    };

    NodeType Holds;
} NodeExpr;

typedef struct NodeStmtExit {
    NodeExpr* Expr;
} NodeStmtExit;

typedef struct NodeStmtDecl {
    TokenSubtype Type;
    NodeExprId*  Ident;
    NodeExpr*    Expr;
} NodeStmtDecl;

typedef struct NodeStmt {
    union {
        NodeStmtExit* Exit;
        NodeStmtDecl* Decl;
    };

    NodeType Holds;
} NodeStmt;

typedef struct NodeRoot {
    NodeStmt** Stats;
    u64        StatsLen;
} NodeRoot;

#endif
