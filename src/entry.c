#include <stdio.h>
#include <stdlib.h>

#include "gen/generate.h"
#include "lexer/token_subtypes.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"

i32 main(i32 argc, char** argv) {
    if (argc <= 1) {
        fputs(
            "Error: No source file to interpret. \nUsage:\tneolang [FILE_TO_INTERPRET]\n", stderr
        );
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
    puts("Begin parsing.");

    puts("\nSource tokens:");
    for (u64 i = 0; i < TokensLen; ++i) {
        Token Token = Tokens[i];

        printf(
            "\tType: %-10s Subtype: %-10s Value: %-10s\n", GetTypeDebugName(Token.Type),
            GetSubtypeDebugName(Token), (char*)Token.Value
        );

        // free(Token.Value);
        // free(Token.Subtype);
    }

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

    const char* Filename = "bin/out.asm";
    FILE*       pOutFile = fopen(Filename, "w");
    if (!pOutFile) {
        fprintf(stderr, "Error: Could not open output file '%s' for write.\n", Filename);
        return EXIT_FAILURE;
    }

    if (!Generate(Tree, pOutFile)) {
        fputs("Error: Code generation failed.\n", stderr);
        return EXIT_FAILURE;
    }

    free(Tree.Stats);
    puts("Code generation complete.");

    if (fclose(pOutFile) != 0) {
        fprintf(stderr, "Error: Could not close output file '%s'.\n", Filename);
        return EXIT_FAILURE;
    }

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    puts("\nCompilation complete!");

    return 0;
}
