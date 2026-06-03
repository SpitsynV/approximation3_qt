#pragma once
#include <vector>
#include <functional>


//void BuildGrid(double a, double b, double c, double d, double p, double q, double r, double s, int nx, int ny,std::vector<double>& val, std::function<double(double, double)> func);
double GetValue(double px, double py, double a, double b, double c, double d, double p, double q, double r, double s,
                const std::vector<double>& val, int nx, int ny);