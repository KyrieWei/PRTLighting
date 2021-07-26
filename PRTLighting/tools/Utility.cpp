#include <iostream>
#include <fstream>
#include <iomanip>

#include "Utility.h"

std::vector<std::string> split(const std::string& line, char c)
{
    std::vector<std::string> vec;
    int start = 0, end = 0;
    while (end != line.length())
    {
        if (line[end] != c)
            end++;
        else
        {
            vec.push_back(line.substr(start, end - start));

            start = end + 1;
            end++;
        }
    }
    if (start < end)
        vec.push_back(line.substr(start, end - start));

    return vec;
}

void generate_mesh(const char* str, int res_x, int res_y, double width, double height)
{
    std::ofstream out(str);

    double length_x = width, length_z = height;

    double delta_x = length_x / res_x;
    double delta_z = length_z / res_y;

    double x, y, z;
    double start_x = -length_x / 2.0, start_z = -length_z / 2.0;

    out << "#vertex_num: " << (res_x + 1) * (res_y + 1) << std::endl;
    out << "#triangle_num: " << res_x * res_y * 2 << std::endl;
    out << std::endl;

    for (int i = 0; i < res_x + 1; i++)
    {
        for (int j = 0; j < res_y + 1; j++)
        {
            x = start_x + delta_x * i;
            y = 0;
            z = start_z + delta_z * j;

            out << "v " << x << " " << y << " " << z << std::endl;
        }
    }

    out << std::endl;

    int ind_0, ind_1, ind_2;

    for (int i = 0; i < res_x; i++)
    {
        for (int j = 0; j < res_y; j++)
        {
            ind_0 = i * (res_y + 1) + j + 1;
            ind_1 = i * (res_y + 1) + j + 2;
            ind_2 = (i + 1) * (res_y + 1) + j + 1;

            out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;

            ind_0 = i * (res_y + 1) + j + 2;
            ind_1 = (i + 1) * (res_y + 1) + j + 2;
            ind_2 = (i + 1) * (res_y + 1) + j + 1;

            out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;
        }
    }

    out.close();
}

//lat 纬线 lon 经线
void generate_ball_mesh(const char* str, int lat, int lon, double radius)
{
    std::ofstream out(str);
    
    double x, y, z;

    //-------------------------------vertex-normal-texcoord------------------------------------
    //Y轴正向极点
    out << "v " << 0 << " " << radius << " " << 0 << std::endl;
    out << "n " << 0 << " " << 1 << " " << 0 << std::endl;
    out << "t " << 0.5 << " " << 1.0 << std::endl;

    //球坐标 
    //phi 偏离z轴正向的角度;
    //theta 偏离x轴正向的角度
    double delta_phi = PI / (double)(lat + 1);
    double delta_theta = 2.0 * PI / (double)(lon - 1);

    double phi = 0, theta = 0, r_sin_phi = 0;

    int vert_num = lat * lon + 2;

    //lat 条纬线
    for (int i = 1; i <= lat; i++)
    {
        phi = delta_phi * i;
        y = radius * cos(phi);
        r_sin_phi = radius * sin(phi);

        //lon 条经线
        for (int j = 0; j < lon; j++)
        {
            double theta = delta_theta * j;
            x = cos(theta) * r_sin_phi;
            z = sin(theta) * r_sin_phi;

            double v = (double)i / (lat + 1);
            double u = (double)j / lon;
            out << "v " << x << " " << y << " " << z << std::endl;
            out << "n " << x / radius << " " << y / radius << " " << z / radius << std::endl;
            out << "t " << 1 - u << " " << 1 - v << std::endl;
            
        }
    }
     
    //Y轴负向极点
    out << "v " << 0 << " " << -radius << " " << 0 << std::endl;
    out << "n " << 0 << " " << -1 << " " << 0 << std::endl;
    out << "t " << 0.5 << " " << 0.0 << std::endl;

    out << std::endl;

    //---------------------------------index------------------------------
    
    int ind_0, ind_1, ind_2;

    //第一层与正极点构成的三角形
    for (int i = 1; i < lon; i++)
    {
        ind_0 = 1;
        ind_1 = i + 1;
        ind_2 = i + 2;

        out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;
    }

    //中间层
    for (int i = 1; i <= lat - 1; i++)
    {
        for (int j = 1; j < lon; j++)
        {
            //两个三角形
            ind_0 = (i - 1) * lon + j + 1;
            ind_1 = ind_0 + lon;
            ind_2 = ind_1 + 1;

            out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;

            ind_0 = (i - 1) * lon + j + 1;
            ind_1 = ind_0 + lon + 1;
            ind_2 = ind_0 + 1;

            out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;
        }
    }

    //最后一层与负极点构成的三角形
    for (int i = 1; i < lon; i++)
    {
        ind_0 = (lat - 1) * lon + i + 1;
        ind_1 = vert_num;
        ind_2 = ind_0 + 1;

        out << "f " << ind_0 << " " << ind_1 << " " << ind_2 << std::endl;
    }

    out.close();
}

void load_mesh(const char* str, float* vertex, unsigned int* index)
{
    std::ifstream m_file(str);
    std::string line;
    std::vector<std::string> split_line;
    std::vector<std::string> split_f;
    if (m_file.is_open())
    {
        int i = 0, j = 0;
        while (std::getline(m_file, line))
        {
            if (line.empty())
                continue;
            if (line[0] == '#')
                continue;

            split_line = split(line, ' ');

            if (split_line[0] == "v")
            {
                vertex[i++] = std::stof(split_line[1]);
                vertex[i++] = std::stof(split_line[2]);
                vertex[i++] = std::stof(split_line[3]);
            }
            else if (split_line[0] == "n" || split_line[0] == "vn")
            {
                vertex[i++] = std::stof(split_line[1]);
                vertex[i++] = std::stof(split_line[2]);
                vertex[i++] = std::stof(split_line[3]);
            }
            else if (split_line[0] == "t" || split_line[0] == "vt")
            {
                vertex[i++] = std::stof(split_line[1]);
                vertex[i++] = std::stof(split_line[2]);
            }
            else if (split_line[0] == "f")
            {
                if (!split_line[1].find('/'))
                {
                    index[j++] = static_cast<unsigned int>(std::stoi(split_line[1]) - 1);
                    index[j++] = static_cast<unsigned int>(std::stoi(split_line[2]) - 1);
                    index[j++] = static_cast<unsigned int>(std::stoi(split_line[3]) - 1);
                }
                else
                {
                    //f contains the same index of v, vn, vt
                    for (int a = 1; a < split_line.size(); a++)
                    {
                        split_f = split(split_line[a], '/');
                        index[j++] = static_cast<unsigned int>(std::stoi(split_f[0]) - 1);
                    }
                }

            }
            else
            {
                continue;
            }
        }
        m_file.close();
    }
    else
    {
        std::cout << "failed to open file " << str << std::endl;
    }
}

void save_vector_txt(const char* str, const std::vector<std::vector<double>>& v)
{
    std::ofstream out(str);

    out << std::setiosflags(std::ios::fixed) << std::setprecision(8) << std::endl;

    for (int i = 0; i < v.size(); i++)
    {
        out << v[i][0] << " " << v[i][1] << " " << v[i][2] << std::endl;
    }

    out.close();


}

unsigned char* loadBMP(std::string filename, int& width, int& height)
{
    int total_bytes;
    unsigned int texture_ID = 0;
    unsigned char* pixels = NULL;
    FILE* pFile;

    fopen_s(&pFile, filename.c_str(), "rb");

    if (pFile == NULL)
    {
        std::cout << "Image not found!" << std::endl;
        return NULL;
    }

    fseek(pFile, 18, SEEK_SET);
    fread(&width, sizeof(width), 1, pFile);
    fread(&height, sizeof(height), 1, pFile);
    fseek(pFile, 54, SEEK_SET);

    total_bytes = (width * 3 + (4 - width * 3 % 4) % 4) * height;

    if ((pixels = (unsigned char*)malloc(total_bytes)) == NULL)
    {
        fclose(pFile);
        return NULL;
    }

    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return NULL;
    }

    return pixels;
}