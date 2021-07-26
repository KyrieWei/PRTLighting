#pragma once
#include <vector>
#include <cmath>

#define PI 3.1415926535897932384626433832795

template<typename T>
T RandT(T _min, T _max);

double random();

std::vector<float> random_dir();
std::vector<float> random_dir(float theta_min, float theta_max);

double SH(int l, int m, double theta, double phi);