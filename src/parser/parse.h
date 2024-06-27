/**
 * @file parser/parse.h
 * @brief Parse.
 * @author github.com/r1ssanen
 */

#ifndef PARSE_H
#define PARSE_H

#include "../lexer/token_types.h"
#include "../types.h"
#include "node_types.h"

NodeTerm*     TryParseTerm();
NodeExpr*     TryParseExpr(u8 MinPrec);
NodeExit*     TryParseExit();
NodeVarDef*   TryParseVarDef();
NodeDecl*     TryParseDecl();
NodeStmtAsgn* TryParseAsgn();
NodeScope*    TryParseScope();
NodeStmtFor*  TryParseFor();
NodeStmt*     TryParseStmt();
NodeRoot*     TryParseRoot();

NodeRoot*     Parse(const Token* Tokens, u64 TokensLen);

#endif
