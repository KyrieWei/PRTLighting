#pragma once
#include <vector>
#include "../tools/Maths.h"
#include "texture.h"

typedef std::vector<double> (*SH_polar_fn)(const texture& text, double theta, double phi);

struct SHSamples
{
	std::vector<double> sph;
	std::vector<double> vec;
	std::vector<double> sh_basis;
};

class harmonics
{
public:
	int sh_order;
	int sqrt_sample_num;

	std::vector<std::vector<double>> sh_coefficents;
	std::vector<SHSamples> samples;

public:

	harmonics() : sh_order(3), sqrt_sample_num(1000) {}
	harmonics(int order, int num) : sh_order(order), sqrt_sample_num(num) {}

	void set_sh_order(int order);
	void set_sample_num(int num);

	void SH_setup_spherical_samples();
	void generate_basis(double x, double y, double z, std::vector<double>& sh_basis);
	void generate_coefficients(SH_polar_fn fn, const texture& text);

	void print_coeff();

};

