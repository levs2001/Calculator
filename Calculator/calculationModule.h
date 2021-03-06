#pragma once
typedef struct {
	double result;
	char name;
}VARIABLES;

double Calculate(char* string, int size, int* err_code, VARIABLES** variable, int* num_var);
int RecordNumbers(char* string, int size_string, double numbers[], int* _size_numbers, double** numbers_pointer, VARIABLES** variable, int* sizeVariables);
int DeleteSpaces(char* string, int* size);
int MakeExpressionsString(char* string, int* _size_string, VARIABLES* variable, int sizeVariables);
int FindPriority(char* string, int* _size_string, int* _priority_index, double* numbers);
int FindIndNumbers(char* string, int* size_string, int priority_index, int* _num1_index, int* _num2_index);
int Compute(char* string, int* size_string, int priority_index, double* numbers, int* size_numbers, int num1_index, int num2_index);
int MinusFilter(char* string, int* size_string);
int FindHooks(char* string, int size, int* openHook, int* closeHook);
int HooksTest(char* string, int* _size_string, int* begin, int* end);
int FindFunc(char* string, int string_size, int* priority_index);
int ComputeFunc(char* string, int* size_string, double* numbers, int func_number, int stringPointOfNum, int* size_numbers);
int PlusFilter(char* string, int* size_string);
int ReadWord(char* string, int string_size, char* func, int* number_point);
int CheckLexic(char* string, int size_string);
int CheckExp(char* string, int size_string);
int FindLog(char* string, int size_string, int* _priority_index);
double Plog(double a, double b);
int CheckList(char* string, int* size_string, VARIABLES** variable, int* num_var);
int FindVariable(char sym, VARIABLES* variable, int sizeVariables);
int CheckForNumFunc(char* string, int size_string, int i);
int  CheckForSign(char* string, int i);
int CheckForGoingStringInNumbers(char* string, int size_string, int i);
