﻿#include <iostream>
#include <iomanip>
#include <time.h>
#include <string>
#include <sstream> 
#include <cmath>
#include <regex>
using namespace std;

/*
    В каждой ячейке двумерного массива хранится значение времени, за которое сигнал эхолота отразился от дна и был принят датчиком. 
    Найти глубину водоема в каждой точке, если плотность воды задается пользователем. 

    Для расчетов будем использовать следующую формулу:

                                        61*(1522.14 + 1.18*(S - 35)*t)
                              H = t * ----------------------------------
                                                   t + 61
                
            где H - расстояние, пройденное волной, является удвоенной глубиной водоема;
                t - время между пуском и приемом волны эхолотом, сек, поступает в программу в массиве;
                S - коэффициент солености воды, промилле, вводится пользователем с клавиатуры.



*/

const int INF = 1e9;
const int Narr = 5;
const int Marr = 5;

// Проверка строки на содержание символов, не являющихся цифрами.
bool checkStr(string s)
{
    regex num_expr("^(-?)(0|([1-9][0-9]*))(\\.[0-9]+)?$"); // Регулярки мои регулярочки.
    if (regex_match(s, num_expr)) return true;
    return false;
}


// Конвертировать строку в double. 
double stringToNum(string s)
{
    double num;
    istringstream buf(s);
    buf >> dec >> num;
    return num;
}

// Х, спец ограничений нет.
double getX(string x1)
{
    double x;
    string s; // Вспомогательная.
    bool ok = false;
    do {
        // Вводим строку. 
        cout << "Введите " << x1 << ": ";
        cin >> s;

        // Проверка, введено ли число.
        if (!checkStr(s))
        {
            cout << "..\nОшибка 1: " << x1 << " должен быть числом\n\n\n";
            continue;
        }

        // Конвертация в double.
        x = stringToNum(s);
        
        ok = true;
    } while (!ok);
    return x;
}

// n, >0.
double getN()
{
    double n;
    bool ok = false;
    do {
        // Вводим с помощью функции для Х.
        double m = getX("соленость в промилле");

        // Дополнительные проверки на "целое" и "положительное".

        //if (m - floor(m)) // Если не целое.
        //{
        //    cout << "..\nОшибка 2: N должен быть целым числом\n\n\n";
        //    continue;
        //}

        if (m < 0)  // Если отрицательное. 
        {
            cout << "..\nОшибка 3: N должен быть больше 0\n\n\n";
            continue;
        }

        if (m > 1000)   // Если слишком большое.
        {
            cout << "..\nОшибка 3: N должен быть меньше 1000\n\n\n";
            continue;
        }
        ok = true;
        n = m;
    } while (!ok);
    return n;
}



int main()
{
    setlocale(LC_ALL, "Russian");

    // Вводим соленость в промилле.
    double sln = getN();


    // Создаем массив, заполняем его случайными значениями времени между 0.00067 сек и 0.06700 сек (примерно соответствуют
    // значениям глубины от 0.5 м до 50м ); Выводим сгенерированные значения в виде матрицы. 

    srand(time(0));
    // Условия вывода знаков после запятой.
    cout << fixed; 
    cout.precision(5);
    // Объявляем массив.
    int n = Narr, m = Marr;
    double arr[Narr][Marr];
    // Цикл с генерацией и выводом.
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            arr[i][j] = (double)((rand() % 6633) + 67) / 100000;
            //cout << arr[i][j] << " ";
            cout << setw(9) << setfill(' ') << internal << arr[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\n\n";


    // Вспомогательные переменные.
    double c61 = 61, c1522 = 1522.14, c118 = 1.18, c35 = 35, c2 = 2, T;

    // Рассчеты на ассемблере и вывод результата. 
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int tmp;
            
            T = arr[i][j];
            
            __asm {


                /* Подставляем соленость и время в формулу и считаем глубину.

                                        61*(1522.14 + 1.18*(S - 35)*t)      1
                              H = t * ---------------------------------- * ---
                                                   t + 61                   2
                */

                finit  
                // Находим числитель. 
                fld sln     // Опускаем соленость в стек.
                fsub c35    // Вещественное вычитание S - 35. 
                fmul c118   // Вещественное умножение 1.18*(S - 35).
                fmul T      // Вещественное умножение 1.18*(S - 35)*t.
                fadd c1522  // Вещественное сложение 1522.14 + 1.18*(S - 35)*t.
                fmul c61    // Вещественное умножение 61*(1522.14 + 1.18*(S - 35)*t).

                // Находим знаменатель. 
                fld T       // Опускаем в стек время.
                fadd c61    // Вещественное сложение t + 61.

                fdiv        // Деление числителя на знаменатель, получаем скорость волны.
                fmul T      // Домножаем скорость на время, чтобы получить глубину. 
                fdiv c2     // Делим на два, чтобы найти реальную глубину водоема. 

                fstp T      // Сохраняем ответ в переменную.


            }   // !asm 
            arr[i][j] = T;


            // Проверка на cpp. 
            //arr[i][j] = T * 61 * (1522.14 + 1.18 * (sln - 35) * T) / (T + 61);

            // Вывод. 
            //cout << arr[i][j] << " ";
            cout << setw(9) << setfill(' ') << internal << arr[i][j] << " ";
        }
        cout << endl;
    }

}   // End.
