#pragma once
#include <vector>
#include <string>

#include "Maths.h"

std::vector<std::string> split(const std::string& line, char c);

void generate_mesh(const char* str, int res_x, int res_y, double width, double height);

void generate_ball_mesh(const char* str, int lat, int lon, double radius);

void load_mesh(const char* str, float* vertex, unsigned int* index);

void save_vector_txt(const char* str, const std::vector<std::vector<double>>& v);

unsigned char* loadBMP(std::string filename, int &width, int &height);