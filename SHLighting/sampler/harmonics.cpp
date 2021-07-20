#include "harmonics.h"
#include <iostream>

void harmonics::set_sh_order(int order)
{
	sh_order = order;
}

void harmonics::set_sample_num(int num)
{
	sqrt_sample_num = num;
}

void harmonics::generate_basis(double x, double y, double z, std::vector<double>& sh_basis)
{
	int n = sh_order * sh_order;
	sh_basis = std::vector<double>(n, 0);

	if (sh_order >= 1)
	{
		sh_basis[0] = 0.5 * sqrt(1.0 / PI);
	}
	if (sh_order >= 2)
	{
		sh_basis[1] = -sqrt(3 / (4 * PI)) * y;
		sh_basis[2] = sqrt(3 / (4 * PI)) * z;
		sh_basis[3] = -sqrt(3 / (4 * PI)) * x;
	}
	if (sh_order >= 3)
	{
		sh_basis[4] = 0.5 * sqrt(15 / PI) * x * y;
		sh_basis[5] = -0.5 * sqrt(15 / PI) * y * z;
		sh_basis[6] = 0.25 * sqrt(5 / PI) * (-x * x - y * y + 2 * z * z);
		sh_basis[7] = -0.5 * sqrt(15 / PI) * z * x;
		sh_basis[8] = 0.25 * sqrt(15 / PI) * (x * x - y * y);
	}
	if (sh_order >= 4)
	{
		sh_basis[9] = -0.25 * sqrt(35 / (2 * PI)) * (3 * x * x - y * y) * y;
		sh_basis[10] = 0.5 * sqrt(105 / PI) * x * y * z;
		sh_basis[11] = -0.25 * sqrt(21 / (2 * PI)) * y * (4 * z * z - x * x - y * y);
		sh_basis[12] = 0.25 * sqrt(7 / PI) * z * (2 * z * z - 3 * x * x - 3 * y * y);
		sh_basis[13] = -0.25 * sqrt(21 / (2 * PI)) * x * (4 * z * z - x * x - y * y);
		sh_basis[14] = 0.25 * sqrt(105 / PI) * (x * x - y * y) * z;
		sh_basis[15] = -0.25 * sqrt(35 / (2 * PI)) * (x * x - 3 * y * y) * x;
	}
}

void harmonics::SH_setup_spherical_samples()
{
	std::cout << "start generating SH samples ..." << std::endl;
	if (sqrt_sample_num <= 0)
		return;

	int i = 0;
	double oneoverN = 1.0 / sqrt_sample_num;

	for (int a = 0; a < sqrt_sample_num; a++)
	{
		for (int b = 0; b < sqrt_sample_num; b++)
		{
			double x = (a + random()) * oneoverN;
			double y = (b + random()) * oneoverN;
			double theta = 2.0 * acos(sqrt(1.0 - x));
			double phi = 2.0 * PI * y;

			SHSamples sample_elem;
			sample_elem.sph = std::vector<double>{ theta, phi, 1.0 };
			sample_elem.vec = std::vector<double>{ sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta) };
			
			
			for (int l = 0; l < sh_order; l++)
			{
				for (int m = -l; m <= l; ++m)
				{
					int index = l * (l + 1) + m;
					sample_elem.sh_basis.push_back(SH(l, m, theta, phi));
				}
			}
			//generate_basis(sample_elem.vec[0], sample_elem.vec[1], sample_elem.vec[2], sample_elem.sh_basis);
			
			//for (int q = 0; q < sample_elem.sh_basis.size(); q++)
			//	std::cout << sample_elem.sh_basis[q] << " ";
			//std::cout << std::endl;
			
			samples.push_back(sample_elem);
		}
	}
	std::cout << "generating SH samples finished" << std::endl;
}

void harmonics::generate_coefficients(SH_polar_fn fn, const texture& text)
{
	std::cout << "start generating SH coefficients ..." << std::endl;

	const double weight = 4.0 * PI;

	int n_coeff = sh_order * sh_order;
	int n_samples = sqrt_sample_num * sqrt_sample_num;

	sh_coefficents = std::vector<std::vector<double>>(n_coeff, std::vector<double>(3, 0));

	for (int i = 0; i < n_samples; i++)
	{
		double theta = samples[i].sph[0];
		double phi = samples[i].sph[1];

		std::vector<double> color = fn(text, theta, phi);

		for (int n = 0; n < n_coeff; n++)
		{
			sh_coefficents[n][0] += color[0] * samples[i].sh_basis[n];
			sh_coefficents[n][1] += color[1] * samples[i].sh_basis[n];
			sh_coefficents[n][2] += color[2] * samples[i].sh_basis[n];

			//if (n == 1)
			//{
			//	std::cout << theta << " " << phi << " " << color[0] << " " << samples[i].sh_basis[n] << " " << sh_coefficents[n][0] << std::endl;
			//}
		} 
	}

	double factor = weight / n_samples;
	for (int i = 0; i < n_coeff; i++)
	{
		sh_coefficents[i][0] *= factor;
		sh_coefficents[i][1] *= factor;
		sh_coefficents[i][2] *= factor;
	}

	std::cout << "generating SH coefficients finished ..." << std::endl;
}

void harmonics::print_coeff()
{
	std::cout << "SH coefficients: " << std::endl;

	for (auto elem : sh_coefficents)
	{
		std::cout << elem[0] << " " << elem[1] << " " << elem[2] << std::endl;
	}

	std::cout << std::endl;
}