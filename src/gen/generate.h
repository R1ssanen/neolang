/**
 * @file gen/generate.h
 * @brief Code generation.
 * @author github.com/r1ssanen
 */

#ifndef GENERATE_H
#define GENERATE_H

#include "../parser/node_types.h"

void        GenWhile(const NodeStmtWhile* While);
void        GenFunDef(const NodeFunDef* FunDef);
void        GenFunDecl(const NodeFunDecl* FunDecl);
void        GenCall(const NodeCall* Call);
void        GenFor(const NodeStmtFor* For);
void        GenTerm(const NodeTerm* Term);
void        GenBinExpr(const NodeBinExpr* BinExpr);
void        GenExpr(const NodeExpr* Expr);
void        GenExit(const NodeStmtExit* Exit);
void        GenVarDef(const NodeVarDef* Def);
void        GenDecl(const NodeDecl* Decl);
void        GenAsgn(const NodeAsgn* Asgn);
void        GenScope(const NodeScope* Scope);
void        GenStmt(const NodeStmt* Stmt);
void        GenRoot(const NodeRoot* Tree);

const char* Generate(const NodeRoot* Tree);

#endif
