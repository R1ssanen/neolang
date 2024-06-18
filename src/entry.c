#include <stdio.h>
#include <stdlib.h>

#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"

i32 main(i32 argc, char** argv) {
    if (argc <= 1) {
        fputs("Error: No source file to compile.\n", stderr);
        return EXIT_FAILURE;
    }

    const char* pFileName = argv[1];
    FILE*       pFile     = fopen(pFileName, "r");

    if (!pFile) {
        fprintf(stderr, "Error: Could not open source file '%s', it may not exist.\n", pFileName);
        return EXIT_FAILURE;
    }

    // get file length
    u64 FileLen = 0;
    fseek(pFile, 0, SEEK_END);
    FileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // read file into buffer
    char* Source = malloc(FileLen + 1);
    fread(Source, sizeof(char), FileLen, pFile);

    if (ferror(pFile) != 0) {
        fprintf(stderr, "Error: Cannot read file '%s'.\n", pFileName);
        return EXIT_FAILURE;
    }
    fclose(pFile);

    // null terminating just to be safe
    Source[FileLen - 1] = '\0';

    puts("\nBegin tokenization.");

    const u64 MAX_TOKENS = 100000;
    Token*    Tokens     = calloc(MAX_TOKENS, sizeof(Token));
    u64       TokensLen  = 0;

    if (!Tokenize((const char*)Source, FileLen, Tokens, &TokensLen)) {
        fputs("Error: Tokenization failed.\n", stderr);
        return EXIT_FAILURE;
    }

    free(Source);
    puts("Tokenization complete.");

    puts("\nSource tokens:");
    for (u64 i = 0; i < TokensLen; ++i) {
        printf(
            "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", GetTypeDebugName(Tokens[i].Type),
            Tokens[i].Subtype, (char*)Tokens[i].Value
        );
    }

    puts("\nBegin parsing.");

    const u64 MAX_NODE_STATEMENTS = 1000;
    NodeRoot  Tree = { .Stats = calloc(MAX_NODE_STATEMENTS, sizeof(NodeStmt)), .StatsLen = 0 };

    if (!Parse(Tokens, TokensLen, &Tree)) {
        fputs("Error: Parsing failed.\n", stderr);
        return EXIT_FAILURE;
    }

    // resize to true size
    Tree.Stats = realloc(Tree.Stats, Tree.StatsLen * sizeof(NodeStmt));

    free(Tokens);
    puts("Parsing complete.");
    puts("Begin code generation.");

    const char* Filepath = "bin/out.asm"; // argv[2];

    if (!Generate(Filepath, &Tree)) {
        fputs("Error: Code generation failed.\n", stderr);
        return EXIT_FAILURE;
    }

    free(Tree.Stats);
    puts("Code generation complete.");

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    puts("\nCompilation complete!");

    return 0;
}
