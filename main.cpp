#include <QApplication>
#include <iostream>
#include <cstdlib>
#include <string>
#include "approximator2d.h"
#include "plotwidget2d.h"
//Подсказка
void printUsage(const char* progName) {
    std::cerr << "Использование:\n"
              << "  Задача 1 (Прямоугольник): " << progName << " 1 a b c d aa bb cc dd nx ny mx my k\n"
              << "  Задача 2 (Круг):          " << progName << " 2 cx cy R nx ny mx my k\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    int TaskNum = 0;
    try {
        TaskNum = std::stoi(argv[1]);
    } catch (...) {
        std::cerr << "Ошибка: Некорректный номер задачи (TaskNum)!" << std::endl;
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    //по умолчанию
    double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
    double aa = 0.0, bb = 0.0, cc = 0.0, dd = 0.0;
    double aaa = 0.0, bbb = 0.0, ccc = 0.0, ddd = 0.0;
    int nx = 0, ny = 0, mx = 0, my = 0, k = 0;

    if (TaskNum == 1) {
        if (argc != 19) {
            std::cerr << "Ошибка: Для задачи 1 требуется 18 аргументов (передано " << argc - 1 << ")\n";
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
        a  = std::stod(argv[2]);
        b  = std::stod(argv[3]);
        c  = std::stod(argv[4]);
        d  = std::stod(argv[5]);
        aa = std::stod(argv[6]);
        bb = std::stod(argv[7]);
        cc = std::stod(argv[8]);
        dd = std::stod(argv[9]);
        aaa = std::stod(argv[10]);
        bbb = std::stod(argv[11]);
        ccc = std::stod(argv[12]);
        ddd = std::stod(argv[13]);
        nx = std::stoi(argv[14]);
        ny = std::stoi(argv[15]);
        mx = std::stoi(argv[16]);
        my = std::stoi(argv[17]);
        k  = std::stoi(argv[18]);
    } 
    else if (TaskNum == 2) {
        if (argc != 10) {
            std::cerr << "Ошибка: Для задачи 2 требуется 8 аргументов (передано " << argc - 1 << ")\n";
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
        // Для круга только cx, cy, R
        aa = std::stod(argv[2]); // cx
        bb = std::stod(argv[3]); // cy
        cc = std::stod(argv[4]); // R
        
        nx = std::stoi(argv[5]);
        ny = std::stoi(argv[6]);
        mx = std::stoi(argv[7]);
        my = std::stoi(argv[8]);
        k  = std::stoi(argv[9]);
    } 
    else {
        std::cerr << "Ошибка: Неизвестный номер задачи: " << TaskNum << std::endl;
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    QApplication app(argc, argv);

    //Для TaskNum=2 a, b, c, d, dd останутся нулями ->OK
    Approximator2D approx(TaskNum, a, b, c, d, aa, bb, cc,dd,aaa,bbb,ccc,ddd, nx, ny, mx, my, k);
    
    PlotWidget2D plot(&approx);
    plot.setWindowTitle("Approx 3");
    plot.show();

    return app.exec();
}