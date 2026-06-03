#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
//void BuildGridR(double cx, double cy, double R, int nx, int ny,std::vector<double>& val, std::function<double(double, double)> func);
double GetValueR(double px, double py, double cx, double cy, double R,
                const std::vector<double>& val, int nx, int ny);