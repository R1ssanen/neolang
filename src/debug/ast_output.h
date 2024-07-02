/**
 * @file debug/ast_output.h
 * @brief Output abstract syntax tree to JSON file.
 * @author github.com/r1ssanen
 */

#ifndef AST_OUTPUT_H
#define AST_OUTPUT_H

#include "../parser/node_types.h"

void OutputInterval(const NodeInterval* Interval);
void OutputScope(const NodeScope* Scope);
void OutputFor(const NodeStmtFor* For);
void OutputTerm(const NodeTerm* Term);
void OutputBinExpr(const NodeBinExpr* BinExpr);
void OutputExpr(const NodeExpr* Expr);
void OutputDef(const NodeVarDef* Def);
void OutputDecl(const NodeDecl* Decl);
void OutputAsgn(const NodeAsgn* Asgn);
void OutputExit(const NodeStmtExit* Exit);
void OutputFor(const NodeStmtFor* For);
void OutputIf(const NodeStmtIf* If);
void OutputStmt(const NodeStmt* Stmt);

void OutputAST(const struct NodeRoot* Tree, const char* Filename);

#endif
