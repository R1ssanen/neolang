/**
 * @file checker/check.h
 * @brief Type-, semantic- and const-checker.
 * @author github.com/r1ssanen
 */

#ifndef CHECK_H
#define CHECK_H

#include "../types.h"

struct NodeRoot;

b8 Check(const struct NodeRoot* Tree);

#endif
