#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "checker/check.h"
#include "debug/ast_output.h"
#include "gen/generate.h"
#include "lexer/tokenize.h"
#include "parser/node_types.h"
#include "parser/parse.h"
#include "types.h"
#include "util/arena.h"
#include "util/error.h"

static clock_t ClockStart;

void           Cleanup(void) {
    PrintErrorStack();
    DestroyMemArena();

    printf("\nCompilation ended: %gs\n", (f64)(clock() - ClockStart) / CLOCKS_PER_SEC);
}

i32 main(i32 argc, char** argv) {
    ClockStart = clock();
    InitMemArena(1024 * 1024 * 25); // 50MiB

    if (atexit(Cleanup) != 0) { RUNTIME_ERR("Failed to register cleanup."); }
    if (argc <= 1) { RUNTIME_ERR("No source file provided."); }

    const char* NeoSource = NULL;
    {
        const char* Path = argv[1];
        FILE*       File = fopen(Path, "r");
        if (!File) { RUNTIME_ERR("Could not open source file '%s', it may not exist.", Path); }

        // get file length
        u64 FileLen = 0;
        fseek(File, 0, SEEK_END);
        FileLen = ftell(File);
        fseek(File, 0, SEEK_SET);

        // read file into buffer
        char* FileContents = Alloc(char, FileLen + 1);
        fread(FileContents, sizeof(char), FileLen, File);

        if (ferror(File) != 0) { RUNTIME_ERR("Cannot read file '%s'.", Path); }
        if (fclose(File) != 0) { RUNTIME_ERR("Could not close source file '%s'.", Path); }

        FileContents[FileLen++] = '\0';
        NeoSource               = FileContents;
    }

    const char* AsmSource = NULL;
    {
        u64    TokenCount = 0;
        Token* Tokens     = Tokenize(NeoSource, &TokenCount);

        for (u64 i = 0; i < TokenCount; ++i) {
            const Token* Token = Tokens + i;
            printf(
                "\tType: %-10s Subtype: 0x%-10X Value: %-10s\n", TOKEN_DEBUG_NAMES[Token->Type],
                Token->Subtype, (char*)Token->Value
            );
        }

        NodeRoot* Tree = Parse(Tokens, TokenCount);
        if (!Tree) { return EXIT_FAILURE; }
        if (!Check(Tree)) { return EXIT_FAILURE; }

        // option to output syntax tree to json
        if (argc >= 3 && strcmp(argv[2], "-fout-ast") == 0) {
            const char* JsonPath = "bin/ast.json";
            OutputAST(Tree, JsonPath);
        }

        AsmSource = Generate(Tree);
    }

    const char* AsmPath = "bin/out.asm"; // argv[2];
    FILE*       AsmFile = fopen(AsmPath, "w");
    if (!AsmFile) { RUNTIME_ERR("Could not open output file '%s' for write.", AsmPath); }

    fputs(AsmSource, AsmFile);
    if (fclose(AsmFile) != 0) { RUNTIME_ERR("Could not close output file '%s'.", AsmPath); }

    system("nasm -felf64 bin/out.asm");
    system("ld -o bin/out bin/out.o");
    return EXIT_SUCCESS;
}
