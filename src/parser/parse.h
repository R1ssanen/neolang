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

NodeStmtIf*   TryParseIf(void);
NodeTerm*     TryParseTerm(void);
NodeExpr*     TryParseExpr(u8 MinPrec);
NodeExit*     TryParseExit(void);
NodeVarDef*   TryParseVarDef(void);
NodeDecl*     TryParseDecl(void);
NodeStmtAsgn* TryParseAsgn(void);
NodeScope*    TryParseScope(void);
NodeStmtFor*  TryParseFor(void);
NodeStmt*     TryParseStmt(void);
NodeRoot*     TryParseRoot(void);

NodeRoot*     Parse(const Token* Tokens, u64 TokensLen);

#endif
