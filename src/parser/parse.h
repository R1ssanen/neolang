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

NodeFunDef*     TryParseFunDef(void);
NodeFunDecl*    TryParseFunDecl(void);
NodeCall*       TryParseCall(void);
NodeInterval*   TryParseInterval(void);
NodeTermNumLit* TryParseNumLit(void);
NodeTermIdent*  TryParseIdent(void);
NodeTerm*       TryParseTerm(void);
NodeExpr*       TryParseBinExpr(u8 MinPrec);
NodeExpr*       TryParseExpr(void);
NodeVarDef*     TryParseVarDef(void);
NodeDecl*       TryParseDecl(void);
NodeAsgn*       TryParseAsgn(void);
NodeScope*      TryParseScope(void);
NodeStmtWhile*  TryParseWhile(void);
NodeStmtExit*   TryParseExit(void);
NodeStmtIf*     TryParseIf(void);
NodeStmtFor*    TryParseFor(void);
NodeStmt*       TryParseStmt(void);
NodeRoot*       TryParseRoot(void);

NodeRoot*       Parse(const Token* Tokens, u64 TokenCount);

#endif
