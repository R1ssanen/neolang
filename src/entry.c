#include <stdio.h>
#include <stdlib.h>

#include "token_subtypes.h"
#include "tokenize.h"
#include "types.h"

i32 main(i32 argc, char** argv) {
    if (argc <= 1) {
        fputs(
            "Error: No source file to interpret. \nUsage:\tneolang [FILE_TO_INTERPRET]\n", stderr
        );
        return -1;
    }

    const char* pFileName = argv[1];
    FILE*       pFile     = fopen(pFileName, "r");

    if (!pFile) {
        fprintf(stderr, "Error: Could not open source file '%s', it may not exist.\n", pFileName);
        return -1;
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
        return -1;
    }
    fclose(pFile);

    // null terminating just to be safe
    Source[FileLen - 1] = '\0';

    puts("Begin tokenization...");

    const u64 MAX_TOKENS = 100000;
    Token*    Tokens     = calloc(MAX_TOKENS, sizeof(Token));
    u64       TokensLen  = 0;

    if (!Tokenize((const char*)Source, FileLen, Tokens, &TokensLen)) {
        fputs("Error: Tokenization failed.\n", stderr);
        return -1;
    }

    free(Source);
    puts("Tokenization complete.");

    puts("\nSource tokens:");
    for (u64 i = 0; i < TokensLen; ++i) {
        Token Token = Tokens[i];

        printf(
            "\tType: %-10s Subtype: %-10s Value: %-10s\n", GetTypeDebugName(Token.Type),
            GetSubtypeDebugName(Token), (char*)Token.Value
        );

        free(Token.Value);
        free(Token.Subtype);
    }

    free(Tokens);

    return 0;
}
