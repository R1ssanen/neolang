/**
 * @file gen/generate.h
 * @brief Code generation.
 * @author github.com/r1ssanen
 */

#ifndef GENERATE_H
#define GENERATE_H

#include "../types.h"
#include "../util/error.h"

struct NodeRoot;

Error* Generate(const char* Filepath, const struct NodeRoot* Tree);

#endif
