#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "stringProcess.h"
#include "calculationModule.h"
#define SIMPLE_STRING 0
#define KOMM_STRING 1
#define EMPTY_STRING 2
#define ERR_STRING 3
#define EOF_STRING 4
#define NOT_ENOUGH_MEMORY 5

#define NO_ANY_END -1
#define END_NEW_STRING 0
#define END_EOF 1



int StringProcess(FILE* Input, FILE* Output) {
    int size = 0, str_type = 0, eof_detector = NO_ANY_END;
    char* string = NULL, * temp_string = NULL;
    char symbol = '\0';
    temp_string = (char*)malloc(++size);
    //temp_string = NULL; //////////
    if (temp_string == NULL) {
        str_type = NOT_ENOUGH_MEMORY;
        while (symbol != '\n' && symbol != EOF) {
            size++;
            symbol = (char)getc(Input);
        }
        string = NULL;


        if (symbol == '\n') {
            eof_detector = END_NEW_STRING;
            PrintAnswer(Output, str_type, size, string);
        }
        else if (symbol == EOF) {
            eof_detector = END_EOF;
            if (size != 2)
                PrintAnswer(Output, str_type, size, string);
        }
        return eof_detector;
    }
    string = temp_string;
    string[0] = (char)getc(Input);

    //Этот if нужен для того, чтобы в случае когда строка состоит из одного EOF ничего не выводить и при '\n' выводить '\n'
    if (string[0] == EOF)
        eof_detector = END_EOF;
    else if (string[0] == '\n') {
        eof_detector = END_NEW_STRING;
        fprintf(Output, "\n");
    }
    else {
        do {
            string = InsertSymbol(Input, string, &size);
            if (size == -1) {
                str_type = NOT_ENOUGH_MEMORY;
                free(string);
                string = NULL;
                PrintAnswer(Output, str_type, size, string);
                eof_detector = END_NEW_STRING;
                return eof_detector;
            }
            if (string[size - 1] == EOF)
                eof_detector = END_EOF;
            else if (string[size - 1] == '\n')
                eof_detector = END_NEW_STRING;

        } while (eof_detector == NO_ANY_END);

        str_type = CheckString(string, size);

        PrintAnswer(Output, str_type, size, string);
    }

    free(string);
    string = NULL;

    return eof_detector;
}

char* InsertSymbol(FILE* Input, char* string, int* size)
{
    char symbol = '\0';
    char* temp_string = NULL;

    (*size)++;
    temp_string = (char*)realloc(string, *size);
    //temp_string = NULL; /////////////
    if (temp_string == NULL) {
        *size = -1;
        while (symbol != '\n' && symbol != EOF)
            symbol = (char)getc(Input);
        return string;
    }
    string = temp_string;
    symbol = (char)getc(Input);
    string[(*size) - 1] = symbol;

    return string;
}


int CheckString(char* string, int size)
{
    //Циклы for до size - 1, потому что последний символ либо EOF либо \n, и я его меняю на \0
    int str_type = SIMPLE_STRING, index = 0;

    if (string[index] < 0) {
        string[size - 1] = '\0';
        str_type = ERR_STRING;
        return str_type;
    }
    for (; index < size - 1 && string[index] >= 0 && isspace(string[index]); index++);
    if (index == size - 1 || (index < size - 2 && string[index] == '/' && string[index + 1] == '/')) { //пустая строка или комментарий
        string[size - 1] = '\0';
        str_type = KOMM_STRING;
        return str_type;
    }
    for (int i = index; i < size - 1; i++) {
        if (string[i] < 0 || !(isspace(string[i]) || isalnum(string[i]) || string[i] == '(' || string[i] == ')' || string[i] == '+' || string[i] == '-' || string[i] == '*' || string[i] == '/' || string[i] == '^' || string[i] == '.' || string[i] == ',')) {
            string[size - 1] = '\0';
            str_type = ERR_STRING;
            return str_type;
        }
    }


    //Проверка на равное число ( и )
    if (CheckHooks(string, size) == ERR_STRING)
        str_type = ERR_STRING;
    //


    if (CkeckDots(string, size) == -1)
        str_type = ERR_STRING;

    string[size - 1] = '\0';

    return str_type;
}


int CkeckDots(char* string, int size_string) {
    int count = 0;
    //проверка точек
    if (string[0] == '.')
        return -1;

    for (int i = 0; i < size_string - 1; i++) {
        if ((string[i] == '.') && (i + 1 == size_string - 1 || isdigit(string[i + 1]) == 0))
            return -1;

        if (isdigit(string[i]) || string[i] == '.')
        {
            if (string[i] == '.')
                count++;
            if (count > 1)
                return -1;
        }
        else
            count = 0;
    }
    //

    return 0;
}


int CheckHooks(char* string, int size) {
    int openHook_count = 0, closeHook_count = 0;
    for (int i = 0; i < size - 1; i++)
        if (string[i] == '(')
            openHook_count++;
        else if (string[i] == ')')
            closeHook_count++;
    if (openHook_count == closeHook_count)
        return EMPTY_STRING;
    else
        return ERR_STRING;
}


void PrintAnswer(FILE* Output, int str_type, int size, char* string) {
    int err_code = 0;
    double answer = 0;
    switch (str_type) {
    case SIMPLE_STRING:
        fprintf(Output, "%s == ", string);
        answer = Calculate(string, size - 1, &err_code);
        if (err_code == 0)
            fprintf(Output, "%g\n", answer);
        else if (err_code == 3)
            fprintf(Output, "%s\n", "ERROR: inf");
        else if (err_code == 4)
            fprintf(Output, "%s\n", "ERROR: sqrt minus");
        else if (err_code == 5)
            fprintf(Output, "%s\n", "ERROR: with pi");
        else if (err_code == 6)
            fprintf(Output, "%s\n", "ERROR: with function names");
        else if (err_code == 7)
            fprintf(Output, "%s\n", "ERROR: with allowable range");
        else if (err_code == 8)
            fprintf(Output, "%s\n", "ERROR: not enough memory for numbers");
        else if (err_code == 9)
            fprintf(Output, "%s\n", "ERROR: problems with digits in string");
        else if (err_code == 10)
            fprintf(Output, "%s\n", "ERROR: no operation");
        else if (err_code == 11)
            fprintf(Output, "%s\n", "ERROR: check lexic");
        else if (err_code == 12)
            fprintf(Output, "%s\n", "ERROR: check exp digits");
        else
            fprintf(Output, "%s\n", "ERROR: characters or brackets");
        break;
    case KOMM_STRING:
        fprintf(Output, "%s\n", string);
        str_type = 0;
        break;
    case EMPTY_STRING:
        fprintf(Output, "%s\n", string);
        str_type = 0;
        break;
    case ERR_STRING:
        fprintf(Output, "%s == %s\n", string, "ERROR: error on first");
        str_type = 0;
        break;
    case NOT_ENOUGH_MEMORY:
        fprintf(Output, "%s\n", "ERROR: not enough memory for string");
        str_type = 0;
        break;
    }
}