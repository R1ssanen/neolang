#include <stdio.h>
#include <stdlib.h>

#include "arena.h"
#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"

// debug
#include "debug/ast_output.h"

i32 main(i32 argc, char** argv) {
    if (argc <= 1) {
        fputs("Error: No source file to compile.\n", stderr);
        return EXIT_FAILURE;
    }

    const char* Filename = argv[1];
    FILE*       File     = fopen(Filename, "r");

    if (!File) {
        fprintf(stderr, "Error: Could not open source file '%s', it may not exist.\n", Filename);
        return EXIT_FAILURE;
    }

    // get file length
    u64 FileLen = 0;
    fseek(File, 0, SEEK_END);
    FileLen = ftell(File);
    fseek(File, 0, SEEK_SET);

    // intialize memory arena with 10mib
    InitMemArena(1024 * 1024 * 10);

    // read file into buffer
    char* Source = Alloc(char, FileLen + 1);
    fread(Source, sizeof(char), FileLen, File);

    if (ferror(File) != 0) {
        fprintf(stderr, "Error: Cannot read file '%s'.\n", Filename);
        return EXIT_FAILURE;
    }
    fclose(File);

    // null terminating just to be safe
    Source[FileLen - 1] = '\0';

    puts("\nBegin tokenization.");

    const u64 MAX_TOKENS = 10000;
    Token*    Tokens     = Alloc(Token, MAX_TOKENS);
    u64       TokensLen  = 0;

    if (!Tokenize((const char*)Source, FileLen, Tokens, &TokensLen)) {
        fputs("Error: Tokenization failed.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Tokenization complete.");
    puts("\nSource tokens:");

    for (u64 i = 0; i < TokensLen; ++i) {
        printf(
            "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", GetTypeDebugName(Tokens[i].Type),
            Tokens[i].Subtype, (char*)Tokens[i].Value
        );
    }

    putchar('\n');
    puts("Begin parsing.");

    const u64 MAX_NODE_STATEMENTS = 1000;
    NodeRoot* Tree                = Alloc(NodeRoot, 1);
    *Tree = (NodeRoot){ .Stats = Alloc(NodeStmt*, MAX_NODE_STATEMENTS), .StatsLen = 0 };

    if (!Parse(Tokens, TokensLen, Tree)) {
        fputs("Error: Parsing failed.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Parsing complete.");
    OutputAST(Tree);

    puts("Begin code generation.");

    const char* Filepath = "bin/out.asm"; // argv[2];

    if (!Generate(Filepath, Tree)) {
        fputs("Error: Code generation failed.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("Code generation complete.");

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");

    puts("\nCompilation complete!");

    DestroyMemArena();

    return EXIT_SUCCESS;
}
