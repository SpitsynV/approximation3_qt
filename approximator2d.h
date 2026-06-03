#pragma once
#include <vector>
#include <functional>
#include <QString>
#include <QColor>
#include "error.h"

class Approximator2D
{
public:
    Approximator2D(int TaskNum, double a, double b, double c, double d, double aa, double bb, double cc, double dd,
                   int nx, int ny, int mx, int my, int k);

    // Пересчёт коэффициентов при изменении параметров
    void rebuild();

    // Значение исходной функции (с возмущением) и всех приближений
    double f   (double x, double y) const;
    double approx1(double x, double y) const;   //Прямоугольник/прямоугольник
    double approx2(double x, double y) const;   //Круг

    double getMaxError1() const;
    double getMaxError2() const;

    // Геттеры
    double a() const { return m_a; }
    double b() const { return m_b; }
    double c() const { return m_c; }
    double d() const { return m_d; }
    double aa() const { return m_aa; }//выреза
    double bb() const { return m_bb; }
    double cc() const { return m_cc; }
    double dd() const { return m_dd; }


    int nx() const { return m_nx; }
    int ny() const { return m_ny; }
    int mx() const { return m_mx; }
    int my() const { return m_my; }
    int k()  const { return m_k;  }
    int p()  const { return m_p;  }
    int scaleExp()  const { return m_scale; }
    int graphMode() const { return m_graphMode; }
    double angle()  const { return m_angle; }   // угол поворота вокруг OZ (рад)
    int TaskNum; //Что решаем- Прямоугольник или круг? 

    // Сеттеры
    void setNx(int nx);
    void setNy(int ny);
    void setP(int p);
    void setScaleExp(int s) { m_scale = s; }
    void setGraphMode(int m){ m_graphMode = m; }
    void setAngle(double a) { m_angle = a; }
    void setTaskNum(int n) { TaskNum = n; rebuild(); }
    // Циклический перебор
    void nextK();
    void nextGraphMode();

    QString functionName() const;

    const std::vector<double>& getX()     const { return m_x; }
    const std::vector<double>& getY()     const { return m_y; }
    const std::vector<double>& getFvals() const { return m_f; }
    //Проверка что точка в области для новых методов
    bool IsInsideDomain(double x, double y) const;
    //
    void BuildGridParallel();
    void BuildGridRParallel();

    // Возвращает функцию для отрисовки (с учётом graphMode)
    // functor(x,y) -> значение
    std::function<double(double, double)> getPlotFunc() const;
    QString getPlotName() const;
    void initGrid();



private:
    
    double m_a, m_b, m_c, m_d;   // область [a,b]x[c,d]
    double m_aa, m_bb, m_cc, m_dd;   // вырез [aa,bb]x[cc,dd]
    int    m_nx, m_ny;            // число точек интерполяции
    int    m_mx, m_my;            // число точек визуализации
    int    m_k;                   // номер функции
    int    m_p;                   // возмущение
    int    m_scale;               // показатель масштаба s
    int    m_graphMode;           // 0..4
    double m_angle;               // угол поворота вокруг OZ

    double m_maxAbsF;             // max|f| без возмущения

    //сетка интерполяции
    std::vector<double> m_x, m_y;
    std::vector<double> m_f;      // m_f[i*m_ny+j] = f(m_x[i], m_y[j]) с возмущением

    
    
};