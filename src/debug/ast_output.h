/**
 * @file debug/ast_output.h
 * @brief Output abstract syntax tree to JSON file.
 * @author github.com/r1ssanen
 */

#ifndef AST_OUTPUT_H
#define AST_OUTPUT_H

#include "../parser/node_types.h"
#include "../util/error.h"

void OutputInterval(const NodeInterval* Interval);
void OutputScope(const NodeScope* Scope);
void OutputFor(const NodeStmtFor* For);
void OutputTerm(const NodeTerm* Term);
void OutputBinExpr(const NodeBinExpr* BinExpr);
void OutputExpr(const NodeExpr* Expr);
void OutputExit(const NodeExit* Exit);
void OutputDef(const NodeVarDef* Def);
void OutputDecl(const NodeDecl* Decl);
void OutputAsgn(const NodeStmtAsgn* Asgn);
void OutputFor(const NodeStmtFor* For);
void OutputStmt(const NodeStmt* Stmt);

b8   OutputAST(const struct NodeRoot* Tree, const char* Filename);

#endif
