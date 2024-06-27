#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug/ast_output.h"
#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"
#include "util/arena.h"
#include "util/error.h"

i32 main(i32 argc, char** argv) {
    clock_t ClockStart = clock();
    InitMemArena(1024 * 1024 * 10);
    InitErrors();

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
    Source[FileLen++]    = '\0';

    const u64 MAX_TOKENS = 10000;
    Token*    Tokens     = Alloc(Token, MAX_TOKENS);
    u64       TokensLen  = 0;

    if (!Tokenize((const char*)Source, FileLen, Tokens, &TokensLen)) {
        PrintErrorStack();
        return EXIT_FAILURE;
    }

    for (u64 i = 0; i < TokensLen; ++i) {
        printf(
            "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", GetTypeDebugName(Tokens[i].Type),
            Tokens[i].Subtype, (char*)Tokens[i].Value
        );
    }

    NodeRoot* Tree = Parse(Tokens, TokensLen);
    if (!Tree) {
        PrintErrorStack();
        return EXIT_FAILURE;
    }

    // option to output syntax tree to json
    if (argc >= 3 && strcmp(argv[2], "-fout-ast") == 0) {
        const char* JsonPath = "bin/ast.json";

        if (!OutputAST(Tree, JsonPath)) {
            PrintErrorStack();
            return EXIT_FAILURE;
        }
    }

    const u64 MAX_ASM_LENGTH = 1010000;
    char*     AsmSource      = Alloc(char, MAX_ASM_LENGTH);

    if (!Generate(Tree, AsmSource)) {
        PrintErrorStack();
        return EXIT_FAILURE;
    }

    const char* AsmPath = "bin/out.asm"; // argv[2];
    FILE*       AsmFile = fopen(AsmPath, "w");
    if (!AsmFile) {
        PRINT_ERROR(_RUNTIME_ERROR, "Could not open output file '%s' for write.", AsmPath);
        return _RUNTIME_ERROR;
    }

    fputs(AsmSource, AsmFile);

    if (fclose(AsmFile) != 0) {
        PRINT_ERROR(_RUNTIME_ERROR, "Could not close output file '%s'.", AsmPath);
        return _RUNTIME_ERROR;
    }

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    DestroyMemArena();

    printf("\nCompilation complete: %fs\n", (f64)(clock() - ClockStart) / CLOCKS_PER_SEC);
    return EXIT_SUCCESS;
}
