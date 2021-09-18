#include <iostream>
#include <time.h>
using namespace std;

/*
В массиве хранятся числа со знаком. Найти максимальное значение и
максимальное по модулю значение, а также индексы этих значений. Если это один
и тот же элемент, то занести в переменную S единицу (иначе 0). [2D] Аналогичным
образом занести 1 в переменную N, если найденные максимумы находятся в
соседних столбцах или строках (иначе 0) Считать, что указанные максимальные
значения в массиве не повторяются.
*/

const int INF = 1e9;
const int Narr = 5;
const int Marr = 5;

int main()
{
    srand(time(0));
    int n = Narr, m = Marr;
    int arr[Narr][Marr];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            arr[i][j] = rand() % 2000 - 1000;
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
    //ma - max elem, mx - x max elem x, my - y max elem y
    //abs - |max elem| (abs), absx - x max elem abs x, absy - y max elem abs y
    int ma = -1000, mx, my, abs = -1, absx, absy, S = 0, N = 0, tmp;
    __asm {
        jmp start

        MaxEl :  // Найти максимальный элемент. 
            lea esi, arr
            mov ecx, n  // Номер строки 
            forArrN :
                mov tmp, ecx    // Сохраняем текущий номер строки. 
                push ecx
                mov ecx, m
                forArrM :
                    mov eax, [esi]
                    cmp eax, ma // if arr[i] > max
                    jg Swap 
                    jmp nex // Конец итерации.
                    Swap :  // Cохраняем максимальный элемент и его поизицию.
                        mov eax, [esi]
                        mov ma, eax     // Сохраняем максимум. 
                        mov ebx, tmp
                        mov mx, ebx     // Сохраняем индексы максимума.
                        mov my, ecx
                    nex :
                    add esi, 4d // Переход к следующему элементу массива. 
                loop forArrM
                pop ecx // Достаем из стека n.
            loop forArrN
        ret

        MaxElAbs :
            lea esi, arr
            mov ecx, n  // Номер строки 
            forArrNAbs :
                mov tmp, ecx    // Сохраняем текущий номер строки. 
                push ecx
                mov ecx, m
                forArrMAbs :
                    mov eax, [esi]
                    cmp eax, 0d // if arr[i] > 0
                    jg pos
                    neg eax // Сохраняем модуль arr[i] в еах
                    pos :

                    cmp eax, abs    // if arr[i] > текущего максимума.
                    jg SwapAbs
                    jmp nexAbs  // Переход к концу итерации
                    SwapAbs :   // Cохраняем максимальный элемент и его поизицию.
                        mov eax, [esi]
                        cmp eax, 0d // if arr[i] > 0
                        jg pos2
                            neg eax // Сохраняем модуль arr[i] в еах
                        pos2 : 
                            mov abs, eax    // Сохраняем максимум. 
                            mov ebx, tmp
                            mov absx, ebx   // Сохраняем индексы максимума.
                            mov absy, ecx
                    nexAbs :
                    add esi, 4d // Переход к следующему элементу массива. 
                loop forArrMAbs
                pop ecx // Достаем из стека n.
            loop forArrNAbs
        ret

        start : // Основная программа, "функция Main"

            // Обнуляем переменные.
            xor eax, eax
            xor ebx, ebx
            xor edx, edx

            // Находим максимальный элемент.
            call MaxEl 

            // Находим максимальный элемент по модулю.
            call MaxElAbs

            mov eax, ma     // Записываем в eax максимальный элемент.
            mov ebx, abs    // Записываем в ebx максимальный по модулю элемент.

            cmp eax, ebx    // if макс. элемент != макс. по модулю элементу.
            jne NotEqual 
                mov S, 1d   // S = 1.

            NotEqual :
            mov eax, mx     // Записываем в eax позицию по x максимального элемента.
            mov ebx, absx   // Записываем в ebx позицию по x максимального элемента по модулю.
            sub eax, ebx    // Вычитание для опрделения , в соседних ли строках.
                cmp eax, 0  // Проверка знака, чтобы узнать, нужно ли брать модуль.
                jg Greater
                neg eax     // Модуль.
            Greater :
            cmp ax, 1   // if |ax| > 1, то проверяем позицию по у.
            jl Col
            mov N, 0d   // Иначе конец программы.
            jmp Exit

            Col :
            mov eax, my     // Записываем в eax позицию по у максимального элемента.
            mov ebx, absy   // Записываем в ebx позицию по у максимального элемента по модулю.
            sub eax, ebx    // Вычитание для опрделения , в соседних ли столбцах.
            cmp eax, 0      // Проверка знака, чтобы узнать, нужно ли брать модуль.
            jg Plus
            neg eax         // Модуль.
            Plus :
            cmp eax, 1  // if |ax| > 1, то конец программы. 
            jg Exit
            mov N, 1d   // Иначе вывод ответа и конец. 
            jmp Exit
            Exit :
    }
    cout << "max= " << ma << " mx= " << n - mx << " my= " << m - my << endl;
    cout << "maxAbs= " << arr[n - absx][m - absy] << " mxAbs= " << n - absx << " myAbs= " << m - absy << endl;
    cout << "N=" << N << endl;
    cout << "S=" << S << endl;
}