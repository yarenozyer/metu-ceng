#ifndef __COLOR_H__
#define __COLOR_H__

#include <iostream>

class Color
{
public:
    double r, g, b;

    Color();
    Color(double r, double g, double b);
    Color(const Color &other);
    friend std::ostream &operator<<(std::ostream &os, const Color &c);

    Color operator+(const Color &other) const {
        return Color(this->r + other.r, this->g + other.g, this->b + other.b);
    }
    Color operator-(const Color &other) const {
        return Color(this->r - other.r, this->g - other.g, this->b - other.b);
    }
    Color operator/(double scalar) const {
        if (scalar != 0.0) {
            return Color(this->r / scalar, this->g / scalar, this->b / scalar);
        } else {
            std::cerr << "Error: Division by zero." << std::endl;
            return *this;
        }
    }
    Color operator*(double scalar) const {
        return Color(this->r * scalar, this->g * scalar, this->b * scalar);
    }
};

#endif