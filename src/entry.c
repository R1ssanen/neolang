#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"
#include "util/arena.h"
#include "util/error.h"

// debug
#include "debug/ast_output.h"

i32 main(i32 argc, char** argv) {
    clock_t ClockStart = clock();
    InitMemArena(1024 * 1024 * 10);

    if (argc <= 1) {
        PRINT_ERROR(_INVALID_ARG, "No source file provided.");
        return _INVALID_ARG;
    }

    const char* Filename = argv[1];
    FILE*       File     = fopen(Filename, "r");
    if (!File) {
        PRINT_ERROR(_RUNTIME_ERROR, "Could not open source file '%s', it may not exist.", Filename);
        return _RUNTIME_ERROR;
    }

    // get file length
    u64 FileLen = 0;
    fseek(File, 0, SEEK_END);
    FileLen = ftell(File);
    fseek(File, 0, SEEK_SET);

    // read file into buffer
    char* Source = Alloc(char, FileLen + 1);
    fread(Source, sizeof(char), FileLen, File);

    if (ferror(File) != 0) {
        PRINT_ERROR(_RUNTIME_ERROR, "Cannot read file '%s'.", Filename);
        return _RUNTIME_ERROR;
    }

    fclose(File);

    // null terminating just to be safe
    Source[FileLen - 1]  = '\0';

    const u64 MAX_TOKENS = 10000;
    Token*    Tokens     = Alloc(Token, MAX_TOKENS);
    u64       TokensLen  = 0;

    Error*    Err        = NULL;
    if ((Err = Tokenize((const char*)Source, FileLen, Tokens, &TokensLen))) {
        fputs(Err->Msg, stderr);
        return Err->Code;
    }

    /*for (u64 i = 0; i < TokensLen; ++i) {
        printf(
            "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", GetTypeDebugName(Tokens[i].Type),
            Tokens[i].Subtype, (char*)Tokens[i].Value
        );
    }*/

    const u64 MAX_NODE_STATEMENTS = 1000;
    NodeRoot* Tree                = Alloc(NodeRoot, 1);
    *Tree = (NodeRoot){ .Stats = Alloc(NodeStmt*, MAX_NODE_STATEMENTS), .StatsLen = 0 };

    if ((Err = Parse(Tokens, TokensLen, Tree))) {
        fputs(Err->Msg, stderr);
        return Err->Code;
    }

    // option to output syntax tree to json
    if (argc >= 3 && strcmp(argv[2], "--output-ast") == 0) {
        const char* JsonPath = "bin/ast.json";

        if ((Err = OutputAST(Tree, JsonPath))) {
            fputs(Err->Msg, stderr);
            return Err->Code;
        }
    }

    const char* AsmPath = "bin/out.asm"; // argv[2];
    if ((Err = Generate(AsmPath, Tree))) {
        fputs(Err->Msg, stderr);
        return Err->Code;
    }

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    DestroyMemArena();

    printf("\nCompilation complete: %fs\n", (f64)(clock() - ClockStart) / CLOCKS_PER_SEC);
    return EXIT_SUCCESS;
}
