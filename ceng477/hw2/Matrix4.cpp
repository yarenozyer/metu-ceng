#include <iomanip>
#include <iostream>
#include "Matrix4.h"

Matrix4::Matrix4()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->values[i][j] = 0;
        }
    }
}

Matrix4::Matrix4(double values[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->values[i][j] = values[i][j];
        }
    }
}

Matrix4::Matrix4(double values[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            this->values[i][j] = values[i][j];
        }

        this->values[i][3] = 0;
    }

    for(int i = 0; i < 3; i++)
    {
        this->values[3][i] = 0;
    }
    this->values[3][3] = 1;
}

Matrix4::Matrix4(const Matrix4 &other)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->values[i][j] = other.values[i][j];
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Matrix4 &m)
{

    os << std::fixed << std::setprecision(6) << "|" << m.values[0][0] << "|" << m.values[0][1] << "|" << m.values[0][2] << "|" << m.values[0][3] << "|"
       << std::endl
       << "|" << m.values[1][0] << "|" << m.values[1][1] << "|" << m.values[1][2] << "|" << m.values[1][3] << "|"
       << std::endl
       << "|" << m.values[2][0] << "|" << m.values[2][1] << "|" << m.values[2][2] << "|" << m.values[2][3] << "|"
       << std::endl
       << "|" << m.values[3][0] << "|" << m.values[3][1] << "|" << m.values[3][2] << "|" << m.values[3][3] << "|";

    return os;
}