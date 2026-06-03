#include "approximator2d.h"
#include "func.h"
#include "task29.h"
#include "task313.h"
#include <cmath>
#include <algorithm>
#include <bits/std_thread.h>

Approximator2D::Approximator2D(int TaskNum, double a, double b, double c, double d, double aa, double bb, double cc, double dd,double aaa, double bbb, double ccc, double ddd,
                               int nx, int ny, int mx, int my, int k)
    : TaskNum(TaskNum), m_a(a), m_b(b), m_c(c), m_d(d), m_aa(aa), m_bb(bb), m_cc(cc), m_dd(dd),m_aaa(aaa), m_bbb(bbb), m_ccc(ccc), m_ddd(ddd)
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
        if (IsInsideRec(x,y,m_aa,m_bb,m_cc,m_dd) ||IsInsideRec(x,y,m_aaa,m_bbb,m_ccc,m_ddd) )
            return false;
        return true;
    }
    else if (TaskNum == 2) {
        // Задача 2: Круг.
        double cx = m_aa;
        double cy = m_bb;
        double R  = m_cc;
        
        double distanceSquared = (x - cx) * (x - cx) + (y - cy) * (y - cy);
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

    if (TaskNum == 1) {//Прямоугольник без 2 прямоугольников
        // --- РАСЧЕТ БЛОКОВ ПО ОСИ X ---
        std::vector<double>boundx={m_aa,m_aaa,m_bb, m_bbb};
        std::sort(boundx.begin(),boundx.end()+1);
        double boundx1=boundx[0];
        double boundx2=boundx[1];
        double boundx3=boundx[2];
        double boundx4=boundx[3];
        /**/
        double L1_x = boundx1 - m_a ; // До дырки
        double L2_x = boundx2 - boundx1; // Дырка 1
        //
        if(L2_x<0){std::swap(boundx1, boundx2); L2_x=-L2_x;}
        double L3_x = boundx3 - boundx2;  // После дырки 1

        //Надо?
        if(L3_x<0){std::swap(boundx2, boundx3); L3_x=-L3_x;}

        double L4_x=boundx4-boundx3;
        if(L4_x<0){std::swap(boundx3, boundx4); L4_x=-L4_x;}

        double L5_x=m_b-boundx4;
        double L_x  = m_b - m_a;   // Полная длина

        int total_intervals_x = m_nx - 1;
        /**/

        int int1_x = std::max(1, static_cast<int>(std::round(total_intervals_x * L1_x / L_x)));
        int int2_x = std::max(1, static_cast<int>(std::round(total_intervals_x * L2_x / L_x)));
        int int3_x = std::max(1, static_cast<int>(std::round(total_intervals_x * L3_x / L_x)));
        int int4_x = std::max(1, static_cast<int>(std::round(total_intervals_x * L4_x / L_x)));
        int int5_x = total_intervals_x - int1_x - int2_x-int3_x-int4_x;
        if (int5_x < 1) int5_x = 1;

        // Блок 1: 
        for (int i = 0; i <= int1_x; ++i) {
            m_x[i] = m_a + i * L1_x / int1_x;
        }
        // Блок 2:
        for (int i = 1; i <= int2_x; ++i) {
            m_x[int1_x + i] = boundx1 + i * L2_x / int2_x;
        }
        // Блок 3: 
        for (int i = 1; i <= int3_x; ++i) {
            m_x[int1_x + int2_x + i] = boundx2 + i * L3_x / int3_x;
        }
        //Блок4
        for (int i = 1; i <= int4_x; ++i) {
            m_x[int1_x + int2_x +int3_x +i] = boundx3 + i * L4_x / int4_x;
        }
        //Блок 5
        for (int i = 1; i <= int5_x; ++i) {
            m_x[int1_x + int2_x +int3_x+int4_x+i] = boundx4 + i * L5_x / int5_x;
        }
//DONE^
        // --- РАСЧЕТ БЛОКОВ ПО ОСИ Y ---
        std::vector<double>boundy={m_cc,m_ccc,m_dd, m_ddd};
        std::sort(boundy.begin(),boundy.end()+1);


        double boundy1=boundy[0];
        double boundy2=boundy[1];
        double boundy3=boundy[2];
        double boundy4=boundy[3];
       
        double L1_y = boundy1 - m_c;  // До дырки

        double L2_y = boundy2 - boundy1; // Дырка 1
        if(L2_y<0){std::swap(boundy1, boundy2); L2_y=-L2_y;}

        double L3_y = boundy3 - boundy2;  // После дырки
        if(L3_y<0){std::swap(boundy2, boundy3); L3_y=-L2_y;}
        double L4_y = boundy4-boundy3;//Дырка 2
        if(L4_y<0){std::swap(boundy3, boundy4); L4_y=-L4_y;}
        double L5_y = m_d-boundy4;
        double L_y  = m_d - m_c;   // Полная длина
        int total_intervals_y = m_ny - 1;

        int int1_y = std::max(1, static_cast<int>(std::round(total_intervals_y * L1_y / L_y)));
        int int2_y = std::max(1, static_cast<int>(std::round(total_intervals_y * L2_y / L_y)));
        int int3_y = std::max(1, static_cast<int>(std::round(total_intervals_y * L3_y / L_y)));
        int int4_y = std::max(1, static_cast<int>(std::round(total_intervals_y * L4_y / L_y)));

        int int5_y = total_intervals_y - int1_y - int2_y-int3_y-int4_y;
        if (int5_y < 1) int5_y = 1;

        // Блок 1: 
        for (int j = 0; j <= int1_y; ++j) {
            m_y[j] = m_c + j * L1_y / int1_y;
        }
        // Блок 2:
        for (int j = 1; j <= int2_y; ++j) {
            m_y[int1_y + j] = boundy1 + j * L2_y / int2_y;
        }
         // Блок 3:
        for (int j = 1; j <= int3_y; ++j) {
            m_y[int2_y+int1_y + j] = boundy2 + j * L3_y / int3_y;
        }
         // Блок 4:
        for (int j = 1; j <= int4_y; ++j) {
            m_y[int3_y+int2_y+int1_y + j] = boundy3 + j * L4_y / int4_y;
        }
        // Блок 5:
        for (int j = 1; j <= int5_y; ++j) {
            m_y[int4_y+int3_y+int2_y+int1_y + j] = boundy4+ j * L5_y / int5_y;
        }
    } 
    else if (TaskNum == 2) {//Круг
        double R = m_cc;
        for (int i = 0; i < m_nx; ++i) {
            m_x[i] = i * R / (m_nx - 1);
        }
        for (int j = 0; j < m_ny; ++j) {
            m_y[j] = j * 2.0 * M_PI / (m_ny - 1);
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
    return GetValue(x,y,m_x,m_y,m_f,m_nx,m_ny,m_aa,m_bb,m_cc,m_dd,m_aaa, m_bbb, m_ccc,m_ddd);
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