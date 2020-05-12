#pragma once
#include <stdio.h>
int StringProcess(FILE* Input, FILE* Output);
char* InsertSymbol(FILE* Input, char* string, int* size);
int CheckString(char* string, int size);
int CheckHooks(char* string, int size);
void PrintAnswer(FILE* Output, int str_type, int size, char* string);
int CkeckDots(char* string, int size_string);

//Change to see