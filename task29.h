#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <functional>
#include <stdexcept>

#include <cmath>
#include <algorithm>

//void BuildGrid(double a, double b, double c, double d, double p, double q, double r, double s, int nx, int ny,std::vector<double>& val, std::function<double(double, double)> func);
bool IsInsideRec(double x, double y, double a, double b, double c, double d);
 
double GetValue(double px, double py, 
                      const std::vector<double>& mx, 
                      const std::vector<double>& my, 
                      const std::vector<double>& mf, 
                      int nx, int ny,
                      double aa, double bb, double cc, double dd, double aaa, double bbb, double ccc, double ddd);