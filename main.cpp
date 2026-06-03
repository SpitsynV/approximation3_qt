#include <QApplication>
#include <iostream>
#include <cstdlib>
#include "task29.h"
#include "approximator2d.h"
#include "plotwidget2d.h"
int main(int argc, char *argv[])
{
    if (argc < 15) {
        std::cerr << "Usage: " << argv[0]
                  << "TaskNum(1 2) a b c d aa bb cc dd nx ny mx my k" << std::endl
                  << "OR TaskNum(1 2) a b c d cx cy R 0 nx ny mx my k" << std::endl;
        return EXIT_FAILURE;
    }
    int TaskNum= std::stoi(argv[1]);
    double a  = std::stod(argv[2]);
    double b  = std::stod(argv[3]);
    double c  = std::stod(argv[4]);
    double d  = std::stod(argv[5]);
    double aa  = std::stod(argv[6]); //или граница выреза или cx
    double bb  = std::stod(argv[7]); //или граница выреза или cy
    double cc  = std::stod(argv[8]);  //или граница выреза или R
    double dd  = std::stod(argv[9]);
    int    nx = std::stoi(argv[10]);
    int    ny = std::stoi(argv[11]);
    int    mx = std::stoi(argv[12]);
    int    my = std::stoi(argv[13]);
    int    k  = std::stoi(argv[14]);
    QApplication app(argc, argv);
    //Требую чтобы границы прямоугольник [a,b]*[c,d] включал в себя круг B((cx,cy),R)
    Approximator2D approx(TaskNum, a, b, c, d, aa, bb, cc, dd, nx, ny, mx, my, k);
    PlotWidget2D plot(&approx);
    plot.setWindowTitle("Approx 3 ");
    plot.show();

    return app.exec();
}