/**
 * @file gen/generate.h
 * @brief Code generation.
 * @author github.com/r1ssanen
 */

#ifndef GENERATE_H
#define GENERATE_H

#include "../parser/node_types.h"

b8 GenFor(const NodeStmtFor* For);
b8 GenTerm(const NodeTerm* Term);
b8 GenBinExpr(const NodeBinExpr* BinExpr);
b8 GenExpr(const NodeExpr* Expr);
b8 GenExit(const NodeExit* Exit);
b8 GenVarDef(const NodeVarDef* Def);
b8 GenDecl(const NodeDecl* Decl);
b8 GenAsgn(const NodeStmtAsgn* Asgn);
b8 GenScope(const NodeScope* Scope);
b8 GenStmt(const NodeStmt* Stmt);
b8 GenRoot(const NodeRoot* Tree);

b8 Generate(const NodeRoot* Tree, char* AsmSource);

#endif
