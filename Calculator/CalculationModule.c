#include "calculationModule.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define PI 3.1415926535897932
#define EXP 2.7182818284590452

//Checking
double Calculate(char* string, int size_string, int* err_code) {
    double answer = 0;
    int size_numbers = 0, priority_index = 0, num1_index = 0, num2_index = 0, operation = 0, findInd_code = 0, compution = 0;
    double* numbers = NULL, * temp_numbers = NULL;

    if (CheckLexic(string, size_string) == -1) {
        *err_code = 11;
        return answer;
    }

    temp_numbers = realloc(numbers, sizeof(double) * (size_numbers + 1));
    //temp_numbers = NULL; /////////////////////
    if (temp_numbers == NULL) {
        *err_code = 8;
        return answer;
    }
    numbers = temp_numbers;


    DeleteSpaces(string, &size_string);

    if (CheckExp(string, size_string) == -1) {
        *err_code = 12;
        free(numbers);
        return answer;
    }

    MinusFilter(string, &size_string);
    PlusFilter(string, &size_string);
    *err_code = RecordNumbers(string, size_string, numbers, &size_numbers, &numbers);
    if (*err_code == 8) {
        free(numbers);
        return answer;
    }
    *err_code = MakeExpressionsString(string, &size_string);
    if (*err_code == 0) {
        while (size_string != 1) {
            operation = FindPriority(string, &size_string, &priority_index, numbers);

            if (operation == -1) {
                *err_code = 1;
                break;
            }
            else if (operation == -2) {
                *err_code = 6;
                break;
            }


            if (size_string == 0) {
                answer = -1;
                free(numbers);
                *err_code = 9;
                return answer;
            }
            else if (size_string != 1) //Эта проверка нужна поскольку FindPriority может изменить размер массива в случае с унарным минусом
              //Сюда добавляем if для случая если FindPriority указал на функцию
                if (operation == 0) {
                    if (priority_index == -1 || priority_index == 0) {
                        *err_code = 10;
                        break;
                    }
                    findInd_code = FindIndNumbers(string, &size_string, priority_index, &num1_index, &num2_index);
                    if (findInd_code != 1) {
                        if (num1_index < 0 || findInd_code == 2) {
                            num1_index = -num1_index;
                            numbers[num1_index] = -numbers[num1_index];
                        }
                        else {
                            if (num2_index < 0) {
                                num2_index = -num2_index;
                                numbers[num2_index] = -numbers[num2_index];
                            }
                            if (Compute(string, &size_string, priority_index, numbers, &size_numbers, num1_index, num2_index) == -1) {
                                *err_code = 3;
                                break;
                            }
                        }
                    }
                    else {
                        *err_code = 2;
                        break;
                    }
                }
                else {
                    compution = ComputeFunc(string, &size_string, numbers, operation, priority_index);
                    if (compution == -1) {
                        *err_code = 7;
                        break;
                    }
                    else if (compution == -2) {
                        *err_code = 6;
                        break;
                    }

                }
        }

        answer = numbers[0];

        if (isinf(answer) != 0)
            *err_code = 3;

        if (isnan(answer) != 0)
            *err_code = 4;

        if (isdigit(string[0]) == 0 && (*err_code) == 0)
            *err_code = 9;
    }
    free(numbers);
    return answer;

}



int RecordNumbers(char* string, int size_string, double numbers[], int* _size_numbers, double** numbers_pointer) {
    int size_numbers = 0, point = 0;
    double* temp_numbers = NULL;
    for (int i = 0; i < size_string; i++) {
        if (isdigit(string[i]) != 0 || isdigit(string[i]) == '-' || string[i] == 'p' || (string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == size_string || isalpha(string[i + 1]) == 0))) {
            if (string[i] == 'p' && string[i + 1] == 'i') {
                numbers[0] = PI;
                size_numbers++;
                point = i + 2;
                break;
            }
            else if (string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == size_string || isalpha(string[i + 1]) == 0)) {
                numbers[0] = EXP;
                size_numbers++;
                point = i + 1;
                break;
            }
            else if (isdigit(string[i]) != 0 || isdigit(string[i]) == '-') { //проверить вот этот момент
                numbers[0] = atof(string + i);
                size_numbers++;
                point = i + 1;
                break;
            }

        }
    }
    for (int i = point; i < size_string; i++) {
        if (!(i - 2 >= 0 && isdigit(string[i - 2]) != 0 && string[i - 1] == 'e')) {
            if (string[i] == '+' || string[i] == '-' || string[i] == '/' || string[i] == '*' || string[i] == '^' || string[i] == '(' || string[i] == ')') {
                if (i == size_string - 1 && i != 0) {
                    if (string[i] == ')') {
                        *_size_numbers = size_numbers;
                        *numbers_pointer = numbers;
                        return 0;
                    }
                    else {
                        *_size_numbers = size_numbers;
                        *numbers_pointer = numbers;
                        return 3;
                    }
                }
                while ((isdigit(string[i]) == 0 && i < size_string - 1) && !(i < size_string - 1 && string[i] == 'p' && string[i + 1] == 'i') && !(string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == size_string || isalpha(string[i + 1]) == 0)))
                    i++;
                if (i != size_string) {
                    if (i < size_string - 1 && string[i] == 'p' && string[i + 1] == 'i') {
                        temp_numbers = realloc(numbers, sizeof(double) * (size_numbers + 1));
                        //temp_numbers = NULL; /////////////////////
                        if (temp_numbers == NULL) {
                            (*numbers_pointer) = numbers;
                            return 8;
                        }
                        numbers = temp_numbers;

                        numbers[size_numbers] = PI;
                        size_numbers++;
                    }
                    else if (string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == size_string || isalpha(string[i + 1]) == 0)) {
                        temp_numbers = realloc(numbers, sizeof(double) * (size_numbers + 1));
                        //temp_numbers = NULL; /////////////////////
                        if (temp_numbers == NULL) {
                            (*numbers_pointer) = numbers;
                            return 8;
                        }
                        numbers = temp_numbers;

                        numbers[size_numbers] = EXP;
                        size_numbers++;
                    }
                    else {
                        temp_numbers = realloc(numbers, sizeof(double) * (size_numbers + 1));
                        //temp_numbers = NULL; /////////////////////
                        if (temp_numbers == NULL) {
                            (*numbers_pointer) = numbers;
                            return 8;
                        }
                        numbers = temp_numbers;

                        numbers[size_numbers] = atof(string + i);
                        size_numbers++;
                    }
                }
            }
        }
    }
    (*_size_numbers) = size_numbers;
    (*numbers_pointer) = numbers;
    return 0;
}


int DeleteSpaces(char* string, int* _size_string) {
    int spaces_counter = 0;
    for (int i = 0; i < *_size_string + 1; i++)
        if (isspace(string[i]))
            spaces_counter++;
        else
            string[i - spaces_counter] = string[i];
    (*_size_string) -= spaces_counter;
    return 0;
}

//Здесь
int MakeExpressionsString(char* string, int* _size_string) {
    for (int i = 0; i < *_size_string; i++) {
        if (isdigit(string[i]) != 0 || string[i] == 'p' || (string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == (*_size_string) || isalpha(string[i + 1]) == 0))) {
            if (string[i] == 'p') {
                if (i < (*_size_string) - 1) {
                    if (string[i + 1] == 'i') {
                        string[i] = '0';
                        string[i + 1] = ' ';
                        i += 2;
                    }
                    else return 5;
                }
                else return 5;
            }
            else if (string[i] == 'e' && (i == 0 || isdigit(string[i - 1]) == 0) && (i + 1 == (*_size_string) || isalpha(string[i + 1]) == 0)) {
                string[i] = '0';
                i++;
            }
            else {
                string[i] = '0';
                i++; //////////////////////////////////Перепроверить этот цикл
                while (i < *_size_string && (isdigit(string[i]) != 0 || string[i] == '.' || (string[i] == 'e' && (i < (*_size_string) - 1 && (string[i + 1] == '-' || string[i + 1] == '+' || isdigit(string[i + 1]) != 0))))) {
                    if (string[i] == 'e' && (string[i + 1] == '-' || string[i + 1] == '+')) {
                        string[i] = ' ';
                        string[i + 1] = ' ';
                        i += 2;
                    }
                    else {
                        string[i] = ' ';
                        i++;
                    }
                }
            }
        }
    }
    DeleteSpaces(string, _size_string);
    return 0;
}



int FindPriority(char* string, int* _size_string, int* _priority_index, double* numbers) {
    int priority_index = -1, begin = 0, end = *_size_string, hooksState = 0, func_number = -1;

    while ((hooksState = HooksTest(string, _size_string, &begin, &end)) != 0)
        if (hooksState == 2)
            return -1;

    for (int i = begin; i < end; i++) {
        if (string[i] == '+' || string[i] == '-') {
            priority_index = i;
            break;
        }
    }

    for (int i = begin; i < end; i++) {
        if (string[i] == '/' || string[i] == '*') {
            priority_index = i;
            break;
        }
    }
    for (int i = end - 1; i > begin; i--) {
        if (string[i] == '^') {
            priority_index = i;
            break;
        }
    }

    func_number = FindFunc(string + begin, end - begin, _priority_index);
    if (func_number == -1)
        return -2;
    if (func_number != 0) {
        *_priority_index = (*_priority_index) + begin;
        return func_number;
    }
    *_priority_index = priority_index;

    if (priority_index == 0) {
        if (string[0] == '-') {
            numbers[0] = -numbers[0];
            string[0] = ' ';
            DeleteSpaces(string, _size_string);
            FindPriority(string, _size_string, _priority_index, numbers);
        }
        else
            return -1;
    }

    return 0;
}



int ReadWord(char* string, int string_size, char* func, int* number_point) {
    int string_point = 0, func_point = 0;

    do {
        func[0] = string[string_point];
        string_point++;
    } while (string_point < string_size && isalpha(func[0]) == 0);

    if (isalpha(func[0]) != 0)
        func_point++;
    else
        return -1;

    do {
        func[func_point] = string[string_point];
        func_point++;
        string_point++;
    } while (func_point < 7 && string_point < string_size && isalpha(func[func_point - 1]) != 0);
    *number_point = string_point - 1;
    func[func_point - 1] = '\0';
    return 0;
}


//Здесь если слово больше
int FindFunc(char* string, int string_size, int* priority_index) {
    char func[8] = "";
    int number_point = 0;
    if (ReadWord(string, string_size, func, &number_point) == 0) {
        *priority_index = number_point;
        if (strcmp(func, "sin") == 0)
            return 1;
        else if (strcmp(func, "cos") == 0)
            return 2;
        else if (strcmp(func, "tg") == 0)
            return 3;
        else if (strcmp(func, "ctg") == 0)
            return 4;
        else if (strcmp(func, "arcsin") == 0)
            return 5;
        else if (strcmp(func, "arccos") == 0)
            return 6;
        else if (strcmp(func, "arctg") == 0)
            return 7;
        else if (strcmp(func, "sqrt") == 0)
            return 8;
        else if (strcmp(func, "ln") == 0)
            return 9;
        else if (strcmp(func, "floor") == 0)
            return 10;
        else if (strcmp(func, "ceil") == 0)
            return 11;
        else
            return -1;
        ///Возвращаем это в случае ошибки с обозначениями функций tg, ctg, arcsin, arccos, arctg
    }
    else
        return 0;
}



int FindIndNumbers(char* string, int* size_string, int priority_index, int* _num1_index, int* _num2_index) {
    int err_flag = 0, point = 1, num1_index = -1, num2_index = 0;

    //Ищем индексы чисел из массива, которые должны быть вычислены

    for (int i = 0; i < priority_index; i++)
        if (isdigit(string[i]) != 0)
            num1_index++;
    num2_index = num1_index + 1;

    //Проверяем всё ли ок со знаками и избавляемся от нескольких идущих подряд -
    if (isdigit(string[priority_index - 1]) == 0) {
        if (string[priority_index] == '-') {
            string[priority_index] = ' ';
            DeleteSpaces(string, size_string);
            num1_index = -1;
            for (int i = 0; i <= priority_index; i++)
                if (isdigit(string[i]) != 0)
                    num1_index++;
            //num2_index = num1_index + 1;
            num1_index = -num1_index;
            *_num1_index = num1_index;
            return 2;
        }
        else {
            err_flag = 1;
            return err_flag;
        }
    }
    //////////////////////////здесь
    while (isdigit(string[priority_index + point]) == 0 && priority_index + point < *size_string) {
        if (string[priority_index + point] != '-') {
            err_flag = 1;
            break;
        }
        else {
            num2_index = -num2_index;
            string[priority_index + point] = ' ';
        }
        point++;
    }

    *_num1_index = num1_index;
    *_num2_index = num2_index;

    DeleteSpaces(string, size_string);
    return err_flag;
}


int Compute(char* string, int* size_string, int priority_index, double* numbers, int* size_numbers, int num1_index, int num2_index) {
    double num1 = 0, num2 = 0, answer = 0;
    num1 = numbers[num1_index];
    num2 = numbers[num2_index];

    switch (string[priority_index]) {
    case '^':
        answer = pow(num1, num2);
        if (isnan(answer) != 0)
            return -1;
        break;
    case '*':
        answer = num1 * num2;
        break;
    case '/':
        if (num2 == 0)
            return -1;
        answer = num1 / num2;
        break;
    case '+':
        answer = num1 + num2;
        break;
    case '-':
        answer = num1 - num2;
        break;
    }

    //Меняем массив записывая туда ответ вместо двух слагаемых
    ///здесь что-то не так 
    numbers[num1_index] = answer;
    for (int i = 0; i < *size_numbers - num2_index - 1; i++)
        numbers[num2_index + i] = numbers[num2_index + i + 1];
    (*size_numbers)--;
    //Меняем строку
    string[priority_index] = ' ';
    string[priority_index + 1] = ' ';

    DeleteSpaces(string, size_string);
    return 0;
}


int ComputeFunc(char* string, int* size_string, double* numbers, int func_number, int stringPointOfNum) {
    int pointInNumbers = -1, minus_flag = 0;
    //Проверка на операция-0 и заносим - в numbers
    if (string[stringPointOfNum] == '-') {
        string[stringPointOfNum] = ' ';
        DeleteSpaces(string, size_string);
        minus_flag = 1;
    }

    for (int i = 0; i <= stringPointOfNum; i++)
        if (isdigit(string[i]) != 0)
            pointInNumbers++;
    if (pointInNumbers < 0)
        return -2;

    if (minus_flag == 1) {
        numbers[pointInNumbers] = -numbers[pointInNumbers];
    }

    switch (func_number) {
    case 1: //sin
        numbers[pointInNumbers] = sin(numbers[pointInNumbers]);
        for (int i = 1; i <= 3; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 2: //cos
        numbers[pointInNumbers] = cos(numbers[pointInNumbers]);
        for (int i = 1; i <= 3; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 3: //tg
        numbers[pointInNumbers] = tan(numbers[pointInNumbers]);
        for (int i = 1; i <= 2; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 4: //ctg
        if (numbers[pointInNumbers] == 0)
            return -1;
        numbers[pointInNumbers] = 1 / tan(numbers[pointInNumbers]);
        for (int i = 1; i <= 3; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 5: //arcsin
        if (fabs(numbers[pointInNumbers]) > 1)
            return -1;
        numbers[pointInNumbers] = asin(numbers[pointInNumbers]);
        for (int i = 1; i <= 6; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 6: //arccos
        if (fabs(numbers[pointInNumbers]) > 1)
            return -1;
        numbers[pointInNumbers] = acos(numbers[pointInNumbers]);
        for (int i = 1; i <= 6; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 7: //arctg
        numbers[pointInNumbers] = atan(numbers[pointInNumbers]);
        for (int i = 1; i <= 5; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 8: //sqrt
        if (numbers[pointInNumbers] < 0)
            return -1;
        numbers[pointInNumbers] = pow(numbers[pointInNumbers], 0.5);
        for (int i = 1; i <= 4; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 9: //ln
        if (numbers[pointInNumbers] <= 0)
            return -1;
        numbers[pointInNumbers] = log(numbers[pointInNumbers]);
        for (int i = 1; i <= 2; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 10: //floor
        numbers[pointInNumbers] = floor(numbers[pointInNumbers]);
        for (int i = 1; i <= 5; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    case 11: //ceil
        numbers[pointInNumbers] = ceil(numbers[pointInNumbers]);
        for (int i = 1; i <= 4; i++)
            string[stringPointOfNum - i] = ' ';
        break;
    }

    DeleteSpaces(string, size_string);
    return 0;
}


int MinusFilter(char* string, int* size_string) {
    int minus_count = 0;
    for (int i = 0; i < *size_string; i++) {
        if (string[i] == '-') {
            while (string[i] == '-' && i < *size_string) {
                minus_count++;
                i++;
            }//Здесь
            if (minus_count > 1) {
                if (minus_count % 2 == 0) {
                    if (i - minus_count != 0) {
                        if (string[i - minus_count - 1] == ')' || isdigit(string[i - minus_count - 1]) != 0 || isalpha(string[i - minus_count - 1]) != 0)
                            string[i - minus_count] = '+';
                        else
                            string[i - minus_count] = ' ';
                    }
                    else
                        string[i - minus_count] = ' ';
                    for (int k = minus_count - 1; k > 0; k--)
                        string[i - k] = ' ';
                }
                else
                    for (int k = minus_count; k > 1; k--)
                        string[i - k] = ' ';
            }
            minus_count = 0;
        }
    }
    DeleteSpaces(string, size_string);
    return 0;
}


int PlusFilter(char* string, int* size_string) {
    int plus_count = 0;
    for (int i = 0; i < *size_string; i++) {
        if (string[i] == '+') {
            while (string[i] == '+' && i < *size_string) {
                plus_count++;
                i++;
            }//Здесь
            if (plus_count > 1) {
                if (i - plus_count == 0 || string[i - plus_count - 1] == '(') {
                    string[i - plus_count] = ' ';
                }
                for (int k = plus_count - 1; k > 0; k--)
                    string[i - k] = ' ';
            }
            plus_count = 0;
        }
    }
    DeleteSpaces(string, size_string);
    return 0;
}


int FindHooks(char* string, int size, int* openHook, int* closeHook) {
    for (int i = 0; i < size; i++) {
        if (string[i] == '(')
            *openHook = i;
        if (string[i] == ')') {
            *closeHook = i;
            break;
        }
    }
    if (*openHook == *closeHook)
        return 1;
    else
        return 0;
}


int HooksTest(char* string, int* _size_string, int* begin, int* end) {
    int openHook = -1, closeHook = -1;

    FindHooks(string, *_size_string, &openHook, &closeHook);
    if (openHook != closeHook) {
        if (openHook == -1 || closeHook == -1)
            return 2;
        else if (closeHook - openHook - 1 == 0)
            return 2;
        else {
            if (closeHook - openHook - 1 == 1) {
                if (isdigit(string[openHook + 1]) == 0)
                    return 2;

                if (openHook != 0 && isdigit(string[openHook - 1]) != 0)
                    return 2;
                else
                    string[openHook] = ' ';

                if ((closeHook + 1) != ((*_size_string) - 1) && isdigit(string[closeHook + 1]) != 0)
                    return 2;
                else
                    string[closeHook] = ' ';

                DeleteSpaces(string, _size_string);
                MinusFilter(string, _size_string);
                PlusFilter(string, _size_string);
                *begin = 0;
                *end = *_size_string;
                return 1;
            }
            else {
                *begin = openHook;
                *end = closeHook;
            }
        }
    }
    return 0;
}



int CheckLexic(char* string, int size_string) {
    struct {
        enum {
            SYMBOL,
            DIGIT,
            PLUS,
            DOT,
            EXPONENT,
            NOTYPE
        }type;
        int point;
    }oldZnak, newZnak;

    newZnak.point = 0;
    oldZnak.point = 0;
    newZnak.type = NOTYPE;
    oldZnak.point = NOTYPE;

    for (int i = 0; i < size_string; i++) {

        oldZnak.point = newZnak.point;
        oldZnak.type = newZnak.type;


        if (isdigit(string[i]) != 0) {
            newZnak.type = DIGIT;
            newZnak.point = i;
        }
        else if (oldZnak.type != SYMBOL && (string[i] == 'e' || string[i] == 'E')) {
            newZnak.type = EXPONENT;
            newZnak.point = i;
        }
        else if (isalpha(string[i]) != 0) {
            newZnak.type = SYMBOL;
            newZnak.point = i;
        }
        else if (string[i] == '+') {
            newZnak.type = PLUS;
            newZnak.point = i;
        }
        else if (string[i] == '.') {
            newZnak.type = DOT;
            newZnak.point = i;
        }
        else if (isspace(string[i]) == 0) {
            newZnak.type = NOTYPE;
            newZnak.point = 0;
            oldZnak.type = NOTYPE;
            oldZnak.point = 0;
        }

        if (newZnak.type != NOTYPE && oldZnak.type != NOTYPE)
            if (newZnak.point - oldZnak.point > 1 && (newZnak.type == oldZnak.type || (oldZnak.type == DIGIT && newZnak.type == EXPONENT) || (newZnak.type == DIGIT && oldZnak.type == EXPONENT)))
                return -1;

    }

    return 0;
}



int CheckExp(char* string, int size_string) {
    for (int i = 1; i < size_string; i++) {
        if (string[i] == 'E' && isdigit(string[i - 1]) != 0)
            string[i] = 'e';
        if (string[i] == 'e' && isdigit(string[i - 1]) != 0)
            if (i + 1 < size_string) {
                if (isdigit(string[i + 1]) == 0 && string[i + 1] != '+' && string[i + 1] != '-')
                    return -1;
                if (string[i + 1] == '+' || string[i + 1] == '-') {
                    if (i + 2 < size_string) {
                        if (isdigit(string[i + 2]) == 0)
                            return -1;
                        i += 2;
                    }
                    else
                        return -1;
                }
                else
                    i++;

                while (i < size_string && isdigit(string[i]) != 0)
                    i++;
                if (i < size_string && string[i] != ')' && string[i] != '*' && string[i] != '/' && string[i] != '+' && string[i] != '-' && string[i] != '^')
                    return -1;
            }
            else
                return -1;
    }
    return 0;
}