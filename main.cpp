#include <QApplication>
#include <iostream>
#include <cstdlib>
#include <string>
#include "approximator2d.h"
#include "plotwidget2d.h"

// Вынесем подсказку в отдельную функцию для чистоты кода
void printUsage(const char* progName) {
    std::cerr << "Использование:\n"
              << "  Задача 1 (Прямоугольник): " << progName << " 1 a b c d aa bb cc dd nx ny mx my k\n"
              << "  Задача 2 (Круг):          " << progName << " 2 cx cy R nx ny mx my k\n";
}

int main(int argc, char *argv[])
{
    // Проверяем, передали ли хотя бы номер задачи
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

    // Объявляем переменные со значениями по умолчанию
    double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
    double aa = 0.0, bb = 0.0, cc = 0.0, dd = 0.0;
    int nx = 0, ny = 0, mx = 0, my = 0, k = 0;

    // В зависимости от задачи ветвим логику парсинга
    if (TaskNum == 1) {
        if (argc != 15) {
            std::cerr << "Ошибка: Для задачи 1 требуется 14 аргументов (передано " << argc - 1 << ")\n";
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
        nx = std::stoi(argv[10]);
        ny = std::stoi(argv[11]);
        mx = std::stoi(argv[12]);
        my = std::stoi(argv[13]);
        k  = std::stoi(argv[14]);
    } 
    else if (TaskNum == 2) {
        if (argc != 10) {
            std::cerr << "Ошибка: Для задачи 2 требуется 8 аргументов (передано " << argc - 1 << ")\n";
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
        // Для круга считываем только cx, cy, R
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

    //Для TaskNum=2 «пустые» a, b, c, d, dd останутся нулями
    Approximator2D approx(TaskNum, a, b, c, d, aa, bb, cc, dd, nx, ny, mx, my, k);
    
    PlotWidget2D plot(&approx);
    plot.setWindowTitle("Approx 3");
    plot.show();

    return app.exec();
}