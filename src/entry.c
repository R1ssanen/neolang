#include <stdio.h>
#include <uchar.h>

#include "tokenize.h"
#include "types.h"

i32 main(i32 argc, char** argv) {
    // REMOVE LATER!
    argc    = 2;
    argv[1] = "../../testfile.neo";

    if (argc <= 1) {
        fputs("No source file to interpret. \nUsage:\tneolang [FILE_TO_INTERPRET]\n", stderr);
        return -1;
    }

    const char* pFileName = argv[1];
    FILE*       pFile     = fopen(pFileName, "r");

    if (!pFile) {
        fprintf(stderr, "Could not open source file '%s', it may not exist.\n", pFileName);
        return -1;
    }

    const u64 MAX_SOURCE_LEN = 1000000;
    char      pSources[MAX_SOURCE_LEN + 1];

    u64       NewLen = fread(pSources, sizeof(char), MAX_SOURCE_LEN, pFile);
    if (ferror(pFile) != 0) {
        fputs("Error reading file", stderr);
        return -1;
    }

    pSources[NewLen++] = '\0'; /* Just to be safe. */
    fclose(pFile);

    const u64 MAX_TOKENS = 100000;
    u64       TokensLen  = 0;
    Token     Tokens[MAX_TOKENS];

    puts("Begin tokenization...");
    if (!Tokenize(pSources, NewLen, Tokens, &TokensLen)) {
        fputs("Tokenization failed.", stderr);
        return -1;
    }

    puts("Tokenization complete.");
    puts("\nSource tokens:");

    // for (size_t i = 0; i < TokensLen; ++i) { printf("\t%s\n", GetTokenDebugName(Tokens[i])); }

    return 0;
}
