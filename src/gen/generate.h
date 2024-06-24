/**
 * @file gen/generate.h
 * @brief Code generation.
 * @author github.com/r1ssanen
 */

#ifndef GENERATE_H
#define GENERATE_H

#include "../parser/node_types.h"
#include "../util/error.h"

Error* GenerateTerm(const NodeTerm* Term);
Error* GenerateBinExpr(const NodeBinExpr* BinExpr);
Error* GenerateExpr(const NodeExpr* Expr);
Error* GeneratePut(const NodeStmtPut* Put);
Error* GenerateExit(const NodeStmtExit* Exit);
Error* GenerateDef(const NodeStmtDef* Def);
Error* GenerateDecl(const NodeStmtDecl* Decl);
Error* GenerateAsgn(const NodeStmtAsgn* Asgn);
Error* GenerateStmt(const NodeStmt* Stmt);
Error* GenerateRoot(const NodeRoot* Tree);

Error* Generate(const NodeRoot* Tree, char* AsmSource);

#endif
