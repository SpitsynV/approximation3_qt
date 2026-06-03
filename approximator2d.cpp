#include "approximator2d.h"
#include "func.h"
#include "task29.h"
#include "task313.h"
#include <cmath>
#include <algorithm>
#include <bits/std_thread.h>

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
bool Approximator2D::IsInsideDomain(double x, double y) const
{
    if (TaskNum == 1) {
        if (x < m_a || x > m_b || y < m_c || y > m_d)
        return false;
        // Точка не должна лежать строго внутри выреза
        if (x > m_aa && x < m_bb && y > m_cc && y < m_dd)
            return false;
        return true;
    } 
    else if (TaskNum == 2) {
        // Задача 2: Круг.
        double cx = m_aa;
        double cy = m_bb;
        double R  = m_cc;
        
        double distanceSquared = (x - cx) * (x - cx) + (y - cy) * (y - cy);
        
        // Добавляем крошечную погрешность 1e-14 на ошибки округления double на самой границе
        return distanceSquared <= (R * R + 1e-14);
    }
    
    return false;
}
void Approximator2D::BuildGridRParallel()
{
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 1;

    int chunkSize = m_nx / numThreads;
    int remainder = m_nx % numThreads;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    int startIdx = 0;
    for (int t = 0; t < numThreads; ++t) {
        int myCount = chunkSize + (t < remainder ? 1 : 0);
        int endIdx = startIdx + myCount;

        threads.emplace_back([this, startIdx, endIdx]() {
            double cx = m_aa; 
            double cy = m_bb; 

            for (int i = startIdx; i < endIdx; ++i) {
                double r = m_x[i]; // Берем уже готовый радиус из initGrid()
                
                for (int j = 0; j < m_ny; ++j) {
                    double phi = m_y[j]; // Берем уже готовый угол из initGrid()
                    
                    double x = cx + r * std::cos(phi);
                    double y = cy + r * std::sin(phi);
                    
                    m_f[i * m_ny + j] = f(x, y);
                }
            }
        });
        startIdx = endIdx;
    }

    for (auto &th : threads) if (th.joinable()) th.join();
}
void Approximator2D::BuildGridParallel()
{
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 1;

    int chunkSize = m_nx / numThreads;
    int remainder = m_nx % numThreads;
    
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    int startIdx = 0;
    for (int t = 0; t < numThreads; ++t) {
        int myCount = chunkSize + (t < remainder ? 1 : 0);
        int endIdx = startIdx + myCount;

        threads.emplace_back([this, startIdx, endIdx]() {
            for (int i = startIdx; i < endIdx; ++i) {
                // Берем уже вычисленный в initGrid() X
                double x = m_x[i]; 
                
                for (int j = 0; j < m_ny; ++j) {
                    // Берем уже вычисленный в initGrid() Y
                    double y = m_y[j]; 
                    
                    m_f[i * m_ny + j] = f(x, y);
                }
            }
        });

        startIdx = endIdx; // Переходим к следующему блоку
    }

    // Синхронизация
    for (auto &th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}
void Approximator2D::initGrid()
{
    m_x.resize(m_nx);
    m_y.resize(m_ny);

    if (TaskNum == 1) {
        // Задача 1:  декартовы координаты узлов 
        for (int i = 0; i < m_nx; ++i) {
            m_x[i] = m_a + i * (m_b - m_a) / (m_nx - 1);
        }
        for (int j = 0; j < m_ny; ++j) {
            m_y[j] = m_c + j * (m_d - m_c) / (m_ny - 1);
        }
    } 
    else if (TaskNum == 2) {
        // Задача 2 (Круг):
        double R = m_cc;
        for (int i = 0; i < m_nx; ++i) {
            m_x[i] = i * R / (m_nx - 1);           // Сетка по r: от 0 до R
        }
        for (int j = 0; j < m_ny; ++j) {
            m_y[j] = j * 2.0 * M_PI / (m_ny - 1);  // Сетка по phi: от 0 до 2*pi
        }
    }

    size_t requiredSize = static_cast<size_t>(m_nx) * m_ny;
    if (m_f.size() != requiredSize) {
        m_f.resize(requiredSize); 
    }
}
void Approximator2D::rebuild()
{
    initGrid();
    if(TaskNum==1){
        BuildGridParallel();
    }else{
        if(TaskNum==2){
            BuildGridRParallel();
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
    if(!IsInsideDomain(x,y)){
        return NAN;
    }
    double v = GetExactValue(x, y, m_k);
    
    // Возмущение применяется ровно в одном узле, в зависимости от геометрии задачи
    if (m_p != 0 && m_nx > 0 && m_ny > 0) {
        double targetX = 0.0;
        double targetY = 0.0;
        
        if (TaskNum == 1) {
            // Для прямоугольника — центральный узел декартовой сетки
            int midx = m_nx / 2;
            int midy = m_ny / 2;
            targetX = m_x[midx];
            targetY = m_y[midy];
        } else if (TaskNum == 2) {
            // Для круга — строго центр круга
            targetX = m_aa; // cx
            targetY = m_bb; // cy
        }
        
        if (std::abs(x - targetX) < 1e-12 && std::abs(y - targetY) < 1e-12) {
            v += m_p * 0.1 * m_maxAbsF;
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