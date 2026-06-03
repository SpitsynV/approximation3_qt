//Для прямог=угольника со сторонами параллельными осям координат,и прямоугольного выреза внутри него со стронами параллельными осям построить приближение
//функции конечными элементами 1 степени(линейными)
//Вход: [a,b]x[c,d], [p,q]x[r,s], nx, ny, f(x,y) - функция, которую нужно аппроксимировать

#include "task29.h"
// Функция расчета значения для прямоугольной области с вырезом
double GetValue(double px, double py, 
                      const std::vector<double>& mx, 
                      const std::vector<double>& my, 
                      const std::vector<double>& mf, 
                      int nx, int ny,
                      double aa, double bb, double cc, double dd)
{
    // 1. В дырке
    if (px >= aa && px <= bb && py >= cc && py <= dd) {
        return NAN; 
    }

    // 2. Быстрый поиск индексов ячейки [i, i+1] x [j, j+1], в которую попала точка (px, py)
    auto itX = std::lower_bound(mx.begin(), mx.end(), px);
    int i = std::distance(mx.begin(), itX) - 1;
    if (i < 0) i = 0;
    if (i >= nx - 1) i = nx - 2;

    auto itY = std::lower_bound(my.begin(), my.end(), py);
    int j = std::distance(my.begin(), itY) - 1;
    if (j < 0) j = 0;
    if (j >= ny - 1) j = ny - 2;

    double x0 = mx[i],     x1 = mx[i+1];
    double y0 = my[j],     y1 = my[j+1];
    double f00 = mf[i * ny + j];        
    double f10 = mf[(i + 1) * ny + j];      
    double f01 = mf[i * ny + j + 1];      
    double f11 = mf[(i + 1) * ny + j + 1];  

    // 3. Локальные координаты
    double dx = (px - x0) / (x1 - x0);
    double dy = (py - y0) / (y1 - y0);

    // 4.Интерполяция
    if (dy <= dx) {
        // Нижний треугольник (узлы: f00, f10, f11)
        return f00 + (f10 - f00) * dx + (f11 - f10) * dy;
    } else {
        // Верхний треугольник (узлы: f00, f01, f11)
        return f00 + (f11 - f01) * dx + (f01 - f00) * dy;
    }
}