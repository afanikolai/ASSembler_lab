#include <iostream> 
#include <string>
#include <sstream> 
#include <cmath>
#include <regex>
using  namespace std;

// Проверка строки, содержит ли она что то кроме цифр.
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

// Х, спец ограничений нет
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

        // Конвертация в double и проверка, не равен ли Х нулю.
        x = stringToNum(s);
        if (x == 0)
        {
            cout << "..\nОшибка 2: X не должно быть 0\n\n\n";
            continue;
        }
        ok = true;
    } while (!ok);
    return x;
}

// n, >0, целое.
int getN()
{
    int n;
    bool ok = false;
    do {
        // Вводим с помощью функции для Х.
        double m = getX("N"); 

        // Дополнительные проверки на "целое" и "положительное".

        if (m - floor(m)) // Если не целое.
        {
            cout << "..\nОшибка 2: N должен быть целым числом\n\n\n";
            continue;
        }

        if (m < 1)  // Если не положительное. 
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
        n = (int)m;
    } while (!ok);
    return n;
}

// Точность, >0
double getE()
{
    double e;
    bool ok = false;
    do {
        // Вводим с помощью функции для Х.
        e = getX("eps");

        if (e <= 0) // Если отрицательное. 
        {
            cout << "..\nОшибка 4: Eps должен быть больше 0\n\n\n";
            continue;
        }
        if (abs((e > 99999)) || abs((e < 0.000001)))    // На диапазон.
        {
            cout << "..\nОшибка: eps не должен быть слишком маленьким или слишком большим числом\n\n\n";
            continue;
        }
        ok = true;
    } while (!ok);
    return e;
}


// -ln(abs(2*sin(x/2)));
double Task1(double x) {
    double ans = 0, part1 = x, part2 = 0, numdiv = 2, one = 1, two = 2, minus = -1;
    double t = 0;
    __asm {
        finit       // Инициализация.
        fld one     // Опускаем в стек 1.
        fld x       // Опускаем в стек х.
        fld two     // Опускаем в стек 2.
        fdiv        // Деление х на 2.
        fsin        // sin x/2.
        fmul two    // 2 * sin x/2.
        fabs        // Модуль 2 * sin x/2.
        
        // log2(x) / log2(e)  =  ln(x)
        
        fyl2x       // Логарифм последнего значения, выгружаем из стека единицу. 
        fldl2e      // Загружаем в стек log2(e)
        fdiv        // Деление.
        fmul minus  // Меняем знак.
        fstp ans    // Сохраняем ответ в переменную ans.
    }
    return ans;
}

// Проверка на плюсах.
double TestCPPTask1(double x) {
    return -log(abs(2 * sin(x / 2)));
}


// cos(nx)/n
double TestCPPTask2N(double x, int n)
{
    double ans = 0;
    for (int i = 1; i <= n; ++i) {
        ans += cos(x * double(i)) / double(i);
    }
    return ans;
}

// cos(nx)/n
// Sum(cos(x*n)/n, (n, 1, inf))
double TestCPPTask2Eps(double x, double eps)
{
    int iter = 1;
    double ans = 0, prev = 1000;
    while (abs(prev - ans) > eps) {
        prev = ans;
        ans += cos(x * double(iter)) / double(iter);
        iter++;
    }
    return ans;
}

// cos(nx)/n
double Task2N(double x, int n)
{
    n++;
    double ans = 0;
    int iter = 1;   // Счетчик.
    __asm {
        mov ecx, 1  // i.        
        calc :
        finit               // Инициализация сопроцессора.
            mov iter, ecx

            fld x           // Загружаем х в стек.
            fimul iter      // Целочисленное умножение х на счетчик.
            fcos            // cos(nx).

            fidiv iter      // cos(nx)/n.
            fadd ans        // Вещественное сложение вершины стека с ans.
            fstp ans        // Перезаписываем ans.
            inc ecx         // i++.
            cmp ecx, n      // if (ecx != n) => goto calc.
            jne calc
    }
    return ans;
}

// cos(nx)/n
double Task2Eps(double x, double eps)
{
    double ans = 0, tmpans, prev = 1000, dif = 0;
    int iter = 1;
    __asm {
        mov ecx, 1 // i.
        calc :
        finit

            mov iter, ecx
            fld x       // Загружаем х в стек.
            fimul iter  // Целочисленное умножение х на счетчик.
            fcos        // cos(nx).

            fidiv iter  // cos(nx)/n.
            fstp tmpans // Перезаписываем tmpans.

            fld ans     // Загружаем ans в стек.
            fstp prev   // Сохраняем ans в prev.

            fld tmpans  // Загружаем tmpans в стек.
            fadd ans    // Вещественное сложение tmpans с ans.
            fstp ans    // Перезаписываем ans. 

            inc ecx     // i++.

            fld ans     // Загружаем ans в стек.
            fsub prev   // Вещественное вычитание (ans-prev).
            fabs        // Модуль abs(ans-prev).
            fstp dif    // Сохранение в dif.

            fld dif     // Загружаем в стек dif.
            fld eps     // Загружаем в стек Eps.
            fcom st(1)  // Cравнение 
            fstsw ax    // Сохранение содержимого регистра перед безусловным переходом. 
            sahf        // Запись регистра во флаги для сравнения. 
            jb calc     // goto calc.
    }
    return ans;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    double x = getX("x");
    int n = getN();
    double eps = getE();
    cout << endl << "\Y cpp = \t" << TestCPPTask1(x) << endl;
    cout << "Y asm = \t" << Task1(x) << endl << endl;

    cout << "ограничение по N cpp = \t" << TestCPPTask2N(x, n) << endl;
    cout << "ограничение по N asm = \t" << Task2N(x, n) << endl << endl;

    cout << "ограничение по eps cpp = " << TestCPPTask2Eps(x, eps) << endl;
    cout << "ограничение по eps asm = " << Task2Eps(x, eps) << endl;
    return 0;
}