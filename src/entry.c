#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug/ast_output.h"
#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "limits.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"
#include "util/arena.h"
#include "util/error.h"

i32 main(i32 argc, char** argv) {
    clock_t ClockStart = clock();
    InitMemArena(1024 * 1024 * 50); // 50mib

    if (argc <= 1) { ARG_ERR("No source file provided."); }

    const char* Filename = argv[1];
    FILE*       File     = fopen(Filename, "r");
    if (!File) { RUNTIME_ERR("Could not open source file '%s', it may not exist.", Filename); }

    // get file length
    u64 FileLen = 0;
    fseek(File, 0, SEEK_END);
    FileLen = ftell(File);
    fseek(File, 0, SEEK_SET);

    // read file into buffer
    char* Source = Alloc(char, FileLen + 1);
    fread(Source, sizeof(char), FileLen, File);

    if (ferror(File) != 0) { RUNTIME_ERR("Cannot read file '%s'.", Filename); }
    fclose(File);
    Source[FileLen++] = '\0';

    u64    TokenCount = 0;
    Token* Tokens     = Tokenize((const char*)Source, FileLen, &TokenCount);

    for (u64 i = 0; i < TokenCount; ++i) {
        printf(
            "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", GetTypeDebugName(Tokens[i].Type),
            Tokens[i].Subtype, (char*)Tokens[i].Value
        );
    }

    NodeRoot* Tree = Parse(Tokens, TokenCount);
    if (!Tree) {
        PrintErrorStack();
        DestroyMemArena();
        return EXIT_FAILURE;
    }

    // option to output syntax tree to json
    if (argc >= 3 && strcmp(argv[2], "-fout-ast") == 0) {
        const char* JsonPath = "bin/ast.json";
        OutputAST(Tree, JsonPath);
    }

    const char* AsmSource = Generate(Tree);

    const char* AsmPath   = "bin/out.asm"; // argv[2];
    FILE*       AsmFile   = fopen(AsmPath, "w");
    if (!AsmFile) { RUNTIME_ERR("Could not open output file '%s' for write.", AsmPath); }

    fputs(AsmSource, AsmFile);
    if (fclose(AsmFile) != 0) { RUNTIME_ERR("Could not close output file '%s'.", AsmPath); }

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    DestroyMemArena();

    printf("\nCompilation complete: %fs\n", (f64)(clock() - ClockStart) / CLOCKS_PER_SEC);
    return EXIT_SUCCESS;
}
