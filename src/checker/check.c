#include "check.h"

#include "../parser/node_types.h"
#include "../types.h"
#include "../util/assert.h"
#include "../util/error.h"

b8 Check(const NodeRoot* Tree) {
    NASSERT_MSG(Tree, "Null input tree.");

    return true;
}
