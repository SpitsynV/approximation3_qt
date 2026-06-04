#include "task29.h"
#include<cmath>
// Вспомогательная функция интерполяции по 3-м произвольным точкам (МКЭ 1-й степени)
double InterpolateTriangle(double x, double y,
                           double x1, double y1, double f1,
                           double x2, double y2, double f2,
                           double x3, double y3, double f3) {
    double det = (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
    
    if (std::abs(det) < 1e-12) {
        return (f1 + f2 + f3) / 3.0; 
    }
    
    double l1 = ((x - x3) * (y2 - y3) - (x2 - x3) * (y - y3)) / det;
    double l2 = ((x1 - x3) * (y - y3) - (x - x3) * (y1 - y3)) / det;
    double l3 = 1.0 - l1 - l2;
    
    return l1 * f1 + l2 * f2 + l3 * f3;
}
// Построение значений в узлах триангуляции круга
void BuildGridR(double cx, double cy, double R, int nx, int ny,
               std::vector<double>& val, std::function<double(double, double)> func) {
    val.resize(nx * ny);
    double hr = R / (nx - 1);
    double hphi = 2.0 * M_PI / (ny - 1);
    
    for (int i = 0; i < nx; ++i) {
        double r = i * hr;
        for (int j = 0; j < ny; ++j) {
            double phi = j * hphi;
            
            // отображение из вспомогательного прямоугольника в исх круг
            double x = cx + r * std::cos(phi);
            double y = cy + r * std::sin(phi);
            
            val[i * ny + j] = func(x, y);
        }
    }
}

// Получить аппроксимированное значение в произвольной точке круга
//План: отобразить круг в прямоугольник [0, R] x [0, 2*PI], найти ячейку, в которой находится точка, и выполнить интерполяцию по треугольникам
//Отдельно обработать случай когда точка близко к центру
double GetValueR(double px, double py, double cx, double cy, double R,
                const std::vector<double>& val, int nx, int ny) {
    // Переводим точку в локальную систему относительно центра круга
    double dx = px - cx;
    double dy = py - cy;
    
    // Обратное отображение: находим полярные координаты прообраза точки
    double pr = std::sqrt(dx * dx + dy * dy);
    if (pr > R + 1e-9) {
        return NAN; 
    }
    if (pr > R) pr = R; 
    
    double pphi = std::atan2(dy, dx);
    if (pphi < 0) {
        pphi += 2.0 * M_PI; // угол в диапазоне [0, 2*PI]
    }
    
    // Шаги вспомогательной сетки
    double hr = R / (nx - 1);
    double hphi = 2.0 * M_PI / (ny - 1);
    
    // Индексы ячейки во вспомогательном пространстве
    int i = static_cast<int>(pr / hr);
    int j = static_cast<int>(pphi / hphi);
    
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i >= nx - 1) i = nx - 2;
    if (j >= ny - 1) j = ny - 2;
    
    double r0 = i * hr;
    double r1 = (i + 1) * hr;
    double phi0 = j * hphi;
    double phi1 = (j + 1) * hphi;
    
    // Находим реальные (x, y) координаты 4-х вершин полярного четырехугольника
    // Узел 00(i, j)
    double x00 = cx + r0 * std::cos(phi0);
    double y00 = cy + r0 * std::sin(phi0);
    double f00 = val[i * ny + j];
    
    // Узел 10(i+1, j)
    double x10 = cx + r1 * std::cos(phi0);
    double y10 = cy + r1 * std::sin(phi0);
    double f10 = val[(i + 1) * ny + j];
    
    // Узел 01(i, j+1)
    double x01 = cx + r0 * std::cos(phi1);
    double y01 = cy + r0 * std::sin(phi1);
    double f01 = val[i * ny + (j + 1)];
    
    // Узел 11(i+1, j+1)
    double x11 = cx + r1 * std::cos(phi1);
    double y11 = cy + r1 * std::sin(phi1);
    double f11 = val[(i + 1) * ny + (j + 1)];
    
    // --- ИНТЕРПОЛЯЦИЯ НА ТРИАНГУЛЯЦИИ ---
    
    if (i == 0) {
        // ОСОБЫЙ СЛУЧАЙ: Центр круга (r = 0).
        // Линия r=0 схлопнулась в точку (cx, cy). Узлы (00) и (01) совпадают геомерически.
        // Ячейка образует ровно один треугольник: Центр, Узел(1, j), Узел(1, j+1)
        return InterpolateTriangle(px, py, x00, y00, f00, x10, y10, f10, x11, y11, f11);
    } else {
        // ОБЩИЙ СЛУЧАЙ: Внешние кольца.
        // Лок координаты в ячейке
        double tx = (pr - r0) / hr;
        double ty = (pphi - phi0) / hphi;
        
        if (tx + ty <= 1.0) {
            // Нижний треугольник в пространстве отображения
            return InterpolateTriangle(px, py, x00, y00, f00, x10, y10, f10, x01, y01, f01);
        } else {
            // Верхний треугольник в пространстве отображения
            return InterpolateTriangle(px, py, x10, y10, f10, x01, y01, f01, x11, y11, f11);
        }
    }
}

