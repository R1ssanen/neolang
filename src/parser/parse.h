/**
 * @file parser/parse.h
 * @brief Parse.
 * @author github.com/r1ssanen
 */

#ifndef PARSE_H
#define PARSE_H

#include "../lexer/token_types.h"
#include "../types.h"
#include "../util/error.h"
#include "node_types.h"

Error* ParseTerm(NodeTerm* Term);
Error* ParseBinExpr(NodeBinExpr* BinExpr);
Error* ParseExpr(NodeExpr* Expr);
Error* ParsePut(NodeStmtPut* Put);
Error* ParseExit(NodeStmtExit* Exit);
Error* ParseDef(NodeStmtDef* Def);
Error* ParseDecl(NodeStmtDecl* Decl);
Error* ParseAsgn(NodeStmtAsgn* Asgn);
Error* ParseStmt(NodeStmt* Stmt);
Error* ParseRoot(NodeRoot* Tree);

Error* Parse(const Token* Tokens, u64 TokensLen, struct NodeRoot* Tree);

#endif
