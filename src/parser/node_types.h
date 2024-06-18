#ifndef NODE_TYPES_H
#define NODE_TYPES_H

#include "../lexer/token_types.h"
#include "../types.h"

typedef enum NodeType {
    _NODE_INVALID    = 0,
    _NODE_NUMLIT     = 0x1,
    _NODE_STRLIT     = 0x2,
    _NODE_ID         = 0x4,
    _NODE_EXPR       = 0x8,
    _NODE_STMT       = 0x10,
    _NODE_STMT_EXIT  = 0x20,
    _NODE_STMT_DECL  = 0x40,
    _NODE_STMT_MDECL = 0x80, // mutable declare (var)

} NodeType;

typedef struct NodeExprNumLit {
    Token NumLit;
} NodeExprNumLit;

typedef struct NodeExprStrLit {
    Token StrLit;
} NodeExprStrLit;

typedef struct NodeExprId {
    Token Id;
} NodeExprId;

typedef struct NodeExpr {
    union {
        NodeExprNumLit NumLit;
        NodeExprStrLit StrLit;
        NodeExprId     Id;
    };

    NodeType Holds;
} NodeExpr;

typedef struct NodeStmtExit {
    union {
        NodeExprNumLit IntLit;
        NodeExprId     Id;
    };

    NodeType Holds;
} NodeStmtExit;

typedef struct NodeStmtDecl {
    // NOTE: we don't need the whole token,
    //       only the specific info
    Token    Type;
    Token    Id;
    NodeExpr Expr;
} NodeStmtDecl;

typedef struct NodeStmt {
    union {
        NodeStmtExit Exit;
        NodeStmtDecl Decl;
    };

    NodeType Holds;
} NodeStmt;

typedef struct NodeRoot {
    NodeStmt* Stats;
    u64       StatsLen;
} NodeRoot;

#endif
