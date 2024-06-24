/**
 * @file debug/ast_output.h
 * @brief Output abstract syntax tree to JSON file.
 * @author github.com/r1ssanen
 */

#ifndef AST_OUTPUT_H
#define AST_OUTPUT_H

#include "../parser/node_types.h"
#include "../util/error.h"

void   OutputTerm(const NodeTerm* Term);
void   OutputBinExpr(const NodeBinExpr* BinExpr);
void   OutputExpr(const NodeExpr* Expr);
void   OutputPut(const NodeStmtPut* Put);
void   OutputExit(const NodeStmtExit* Exit);
void   OutputDef(const NodeStmtDef* Def);
void   OutputDecl(const NodeStmtDecl* Decl);
void   OutputAsgn(const NodeStmtAsgn* Asgn);
void   OutputStmt(const NodeStmt* Stmt);

Error* OutputAST(const struct NodeRoot* Tree, const char* Filename);

#endif
