#include "approximator2d.h"
#include "func.h"
#include "task29.h"
#include "task313.h"
#include <cmath>
#include <algorithm>

Approximator2D::Approximator2D(int TaskNum, double a, double b, double c, double d, double aa, double bb, double cc, double dd,
                               int nx, int ny, int mx, int my, int k)
    : TaskNum(TaskNum), m_a(a), m_b(b), m_c(c), m_d(d), m_aa(aa), m_bb(bb), m_cc(cc), m_dd(dd)
    , m_nx(nx), m_ny(ny), m_mx(mx), m_my(my)
    , m_k(k), m_p(0), m_scale(0), m_graphMode(0), m_angle(0.0)
    , m_maxAbsF(0.0)
{
    //сетка для методов
    m_x.resize(m_nx);
    m_y.resize(m_ny);
    m_f.resize(m_nx*m_ny);
    //initGrid();
    if(TaskNum==2){
        //Квадарат в который вписан круг
        m_a=m_aa-m_cc; //cx-R
        m_b=m_aa+m_cc; //cx+R
        m_c=m_bb-m_cc; //cy-R
        m_d=m_bb+m_cc; //cy+R
    }
}

void Approximator2D::initGrid()
{
    // сетка интерполяции по x и y.
    m_x.resize(m_nx);
    m_y.resize(m_ny);
    for (int i = 0; i < m_nx; i++){
        m_x[i] = m_a + i * (m_b - m_a) / (m_nx - 1);
    }
    for (int j = 0; j < m_ny; j++){
        m_y[j] = m_c + j * (m_d - m_c) / (m_ny - 1);
    }
    // значения функции//
    m_f.resize(m_nx * m_ny);
    m_maxAbsF = 0.0;
    for (int i = 0; i < m_nx; i++) {
        for (int j = 0; j < m_ny; j++) {
            if(!IsInsideDomain(m_x[i], m_y[j], m_a, m_b, m_c, m_d, m_aa, m_bb, m_cc, m_dd)){
                m_f[i * m_ny + j] = NAN;
            }else{
                double v = GetExactValue(m_x[i], m_y[j], m_k);
                m_f[i * m_ny + j] = v;
                m_maxAbsF = std::max(m_maxAbsF, std::fabs(v));
            }
        }
    }

    // возмущение: добавляем p*0.1*max|f| к f(x_{nx/2}, y_{ny/2})
    if (m_p != 0) {
        int imid = m_nx / 2;
        int jmid = m_ny / 2;
        m_f[imid * m_ny + jmid] += m_p * 0.1 * m_maxAbsF;
    }
}
void Approximator2D::rebuild()
{
    initGrid();
    if(TaskNum==1){
        BuildGrid(m_a, m_b, m_c, m_d, m_aa, m_bb, m_cc, m_dd, m_nx, m_ny, m_f,
                  [this](double x, double y) { return this->f(x, y); });
    }else{
        if(TaskNum==2){
            BuildGridR(m_aa,m_bb,m_cc,m_nx,m_ny,m_f,[this](double x, double y) { return this->f(x, y); });
        }
    }

    //GetCoefficients6(m_nx,m_ny,m_a,m_b,m_c,m_d,exact,m_c6);


}
void Approximator2D::setP(int p)
{
    m_p = p;
    rebuild();
}
double Approximator2D::getMaxError1() const
{
    if (m_nx < 2 || m_ny<2) return -1.0;
    return maxAbsoluteErrorParallel(m_x[0], m_x[m_nx-1],m_y[0], m_y[m_ny-1],
        [this](double x, double y) { return this->f(x,y); },
        [this](double x, double y) { return this->approx1(x,y); });
}

double Approximator2D::getMaxError2() const
{
    if (m_nx < 2 || m_ny<2) return -1.0;
    return maxAbsoluteErrorParallel(m_a, m_b,m_c, m_d,
        [this](double x, double y) { return this->f(x,y); },
        [this](double x, double y) { return this->approx2(x,y); });
}
        
void Approximator2D::setNx(int nx) { m_nx = std::max(2, nx);

}
void Approximator2D::setNy(int ny) { m_ny = std::max(2, ny);
    
    


}

void Approximator2D::nextK()
{
    m_k = (m_k + 1) % 8;  // k=0..7
}

void Approximator2D::nextGraphMode()
{
    m_graphMode = (m_graphMode + 1) % 3;  // 0..2
}

QString Approximator2D::functionName() const
{
    return QString(FuncName(m_k));
}

// исходная функция с возмущением
double Approximator2D::f(double x, double y) const
{
    if(!IsInsideDomain(x, y, m_a, m_b, m_c, m_d, m_aa, m_bb, m_cc, m_dd)){
        return NAN;
    }
    double v = GetExactValue(x, y, m_k);
    // возмущение применяется только в 1 точке
    if (m_p != 0 && m_nx > 0 && m_ny>0) {
        int midx = m_nx / 2;
        int midy = m_ny/2;
        // Сравнение с mid с учётом погрешности
        if (std::abs(x - m_x[midx]) < 1e-12 * std::max(1.0, std::abs(x))) {
            if(std::abs(y - m_y[midy]) < 1e-12 * std::max(1.0, std::abs(y))){
                v += m_p * 0.1 * m_maxAbsF;
            }
        }
    }
    return v;
}

double Approximator2D::approx1(double x, double y) const
{
    return GetValue(x,y,m_a,m_b,m_c,m_d,m_aa,m_bb,m_cc,m_dd,m_f,m_nx,m_ny);
}
double Approximator2D::approx2(double x, double y) const
{
    return GetValueR(x,y,m_aa,m_bb,m_cc,m_f,m_nx,m_ny);
}



std::function<double(double, double)> Approximator2D::getPlotFunc() const
{
    switch (m_graphMode) {
    case 0:  // функция
        return [this](double x, double y){ return this->f(x, y); };
    case 1:  // приближение по методу  
        if(TaskNum==1) return[this](double x, double y){ return this->approx1(x, y); };
        if(TaskNum==2) return[this](double x, double y){ return this->approx2(x, y); };
    case 2:  // погрешность метода 
        if(TaskNum==1)return [this](double x, double y){ return this->approx1(x, y) - this->f(x, y); };
        if(TaskNum==2)return [this](double x, double y){ return this->approx2(x, y) - this->f(x, y); };
    default:
        return [](double, double){ return 0.0; };
    }
}

QString Approximator2D::getPlotName() const
{
    switch (m_graphMode) {
    case 0: return "f(x,y)";
    case 1: return "Approx1";
    case 2: return "Error1 = P1-f";
    default: return "";
    }
}
//