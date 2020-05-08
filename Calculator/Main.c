// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


// Надо добавить в собственные хэдеры помимо pragma once #endif...
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "stringProcess.h"
#include "calculationModule.h"


int main(int argc, char* argv[]) {
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    FILE* Input = stdin;
    FILE* Output = stdout;

    if (argc == 2) {
        fopen_s(&Input, argv[1], "r");
        if (Input == NULL) {
            fprintf(stdout, "%s", "ERROR: can't open file for reading...");
            return 1;
        }
    }
    else if (argc == 3) {
        fopen_s(&Input, argv[1], "r");
        fopen_s(&Output, argv[2], "w");

        if (Input == NULL) {
            fprintf(stdout, "%s", "ERROR: can't open file for reading...");
            return 1;
        }
        else if (Output == NULL) {
            fprintf(stdout, "%s", "ERROR: can't open file for writing...");
            return 1;
        }
        else {
            //no
        }
    }
    else if (argc > 3) {
        fprintf(stdout, "%s", "ERROR: the count of parametrs isn't correct...");
        return 1;
    }
    else {
        //No
    }

    while (StringProcess(Input, Output) == 0) {};

    fclose(Input);
    fclose(Output);

    return 0;
}