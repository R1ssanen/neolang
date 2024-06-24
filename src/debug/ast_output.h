#ifndef AST_OUTPUT_H
#define AST_OUTPUT_H

#include "../types.h"
#include "../util/error.h"

struct NodeRoot;

Error* OutputAST(const struct NodeRoot* Tree, const char* Filename);

#endif
