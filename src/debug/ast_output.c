#include "ast_output.h"

#include <stdio.h>

#include "../parser/node_types.h"
#include "../types.h"
#include "jWrite.h"

b8 OutputAST(const NodeRoot* Tree) {
    if (!Tree) {
        fputs("DebugError: Null input parameter.\n", stderr);
        return false;
    }

    char AST[1024];
    jwOpen(AST, 1024, JW_ARRAY, JW_COMPACT);

    for (u32 i = 0; i < Tree->StatsLen; ++i) {
        NodeStmt* Stmt = Tree->Stats[i];

        jw_object();

        jw_key("stmt");
        switch (Stmt->Holds) {
        case _NODE_STMT_EXIT: jw_string("exit"); break;
        case _NODE_STMT_DECL: jw_string("decl"); break;
        default: jw_null();
        }

        jwEnd();
    }

    jwEnd();
    jwClose();

    puts("\nAbstract Syntax Tree:");
    printf("\t%s\n", AST);
    putchar('\n');

    FILE* Json = fopen("bin/ast.json", "w");
    if (!Json) {
        fputs("DebugError: Could not open 'bin/ast.json' for write.\n", stderr);
        return false;
    }

    fprintf(Json, "%s", AST);
    fclose(Json);

    return true;
}
