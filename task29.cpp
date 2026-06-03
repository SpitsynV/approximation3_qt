//Для прямог=угольника со сторонами параллельными осям координат,и прямоугольного выреза внутри него со стронами параллельными осям построить приближение
//функции конечными элементами 1 степени(линейными)
//Вход: [a,b]x[c,d], [p,q]x[r,s], nx, ny, f(x,y) - функция, которую нужно аппроксимировать
#include <iostream>
#include <cmath>
#include <functional>
#include <stdexcept>

bool IsInsideDomain(double x, double y, double a, double b, double c, double d, double p, double q, double r, double s) {
    if (x < a || x > b || y < c || y > d)
        return false;
    // Точка не должна лежать строго внутри выреза
    if (x > p && x < q && y > r && y < s)
        return false;
    return true;
}
//Для равномерной сетки мне даже не нужн сами x[i], y[j], а только массив значений в узлах.
void BuildGrid(double a, double b, double c, double d, double p, double q, double r, double s, int nx, int ny,
               std::vector<double>& val, std::function<double(double, double)> func) {
    double hx = (b - a) / (nx - 1);
    double hy = (d - c) / (ny - 1);
    double x=a;
    double y=c;
    for(int i=0; i<nx; i++){
        for(int j=0; j<ny; j++){
            x = a + i*hx;
            y = c + j*hy;
            if(IsInsideDomain(x,y,a,b,c,d,p,q,r,s)){
                val[i*ny+j] = func(x,y);
            } else {
                val[i*ny+j] = NAN; //Точно обьявить что узел не принадлежит области, чтобы потом не использовать его в аппроксимации
            }
        }
    }
}
//Получить значение функции в произвольной точке (px, py). Требование nx, ny >=2 
double GetValue(double px, double py, double a, double b, double c, double d, double p, double q, double r, double s,
                const std::vector<double>& val, int nx, int ny) {
    if (!IsInsideDomain(px, py, a, b, c, d, p, q, r, s)) {
        return NAN;
    }

    double hx = (b - a) / (nx - 1); //количество ячеек равно nx-1 !!!
    double hy = (d - c) / (ny - 1);
    int i = static_cast<int>((px - a) / hx); // номер ячейки по x
    int j = static_cast<int>((py - c) / hy);
    // Проверяем границы
    if(i<0) i=0;
    if(j<0) j=0;
    if (i >= nx - 1) i = nx - 2;
    if (j >= ny - 1) j = ny - 2;
    // Получаем значения в узлах
    double f00 = val[i*ny+j];
    double f10 = val[(i + 1)*ny+j];
    double f01 = val[i*ny+(j + 1)];
    double f11 = val[(i + 1)*ny+(j + 1)];
  //Надо понять в данной ячейке использовать верхний или нижний треугольник
    double x0 = a + i * hx;//координаты левого нижнего угла ячейки
    double y0 = c + j * hy;//координаты левого нижнего угла ячейки
    double x1=x0+hx;//координаты правого верхнего угла ячейки
    double y1=y0+hy;//координаты правого верхнего угла 
    double dx=px-x0;
    double dy=py-y0;
    if(dx*hy + dy*hx <= hx*hy){
        //нижний треугольник, (x0,y0), (x1,y0), (x0,y1)
        double psi1=(px-x1)*(hy)+(py-y0)*hx;
        double psi2=(px-x0)*hy;
        double psi3=(py-y0)*hx;
        double phi1=psi1/(-hx*(hy));
        double phi2=psi2/(hx*hy);
        double phi3=psi3/(hy*hx);
        return f00*phi1 + f10*phi2 + f01*phi3; //Обработка NAN-ов учтена
                                                //NAN + NAN=NAN, NAN*NAN=NAN, NAN+число=NAN, NAN*число=NAN
    } else {
        //верхний треугольник (x1,y0),(x0,y1), (x1,y1), 
        double psi1=-(py-y1)*hx;
        double psi2=(px-x1)*hy;
        double psi3=(px-x1)*hy-(py-y0)*(-hx);
        double phi1=psi1/(hx*hy);
        double phi2=psi2/(-hx*hy);
        double phi3=psi3/(hy*hx);
        return f10*phi1+f01*phi2+f11*phi3;
    }

}
