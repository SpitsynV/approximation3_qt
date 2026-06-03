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
                  << "TaskNum a b c d aa bb cc dd nx ny mx my k" << std::endl;
        return EXIT_FAILURE;
    }
    int TaskNum= std::stoi(argv[1]);
    double a  = std::stod(argv[2]);
    double b  = std::stod(argv[3]);
    double c  = std::stod(argv[4]);
    double d  = std::stod(argv[5]);
    double aa  = std::stod(argv[6]);
    double bb  = std::stod(argv[7]);
    double cc  = std::stod(argv[8]);
    double dd  = std::stod(argv[9]);
    int    nx = std::stoi(argv[10]);
    int    ny = std::stoi(argv[11]);
    int    mx = std::stoi(argv[12]);
    int    my = std::stoi(argv[13]);
    int    k  = std::stoi(argv[14]);
    QApplication app(argc, argv);
    Approximator2D approx(a, b, c, d, aa, bb, cc, dd, nx, ny, mx, my, k);
    PlotWidget2D plot(&approx);
    plot.setWindowTitle("Approx 3 ");
    plot.show();

    return app.exec();
}