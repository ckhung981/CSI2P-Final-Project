#ifndef TRIANGLE_H_INCLUDED
#define TRIANGLE_H_INCLUDED

#include "Shape.h"

/**
 * @see Shape.cpp
 */
class Triangle : public Shape
{
public:
	bool overlap(const Shape &s) const;
	double center_x() const { return x2; }
	double center_y() const { return (y1 + y2) / 2; }
	void update_center_x(const double &x) {
		double dx = x - center_x();
		x1 += dx, x2 += dx, x3 += dx;
	}
	void update_center_y(const double &y) {
		double dy = y - center_y();
		y1 += dy, y2 += dy, y3 += dy;
	}
	const ShapeType getType() const { return ShapeType::TRIANGLE; }

    bool contain(double x, double y) const{
        double AC_AC = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1); 
        double AB_AC = (x2 - x1) * (x3 - x1) + (y2 - y1) * (y3 - y1);
        double AB_AB = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1); 
        double AP_AB = (x - x1) * (x2 - x1) + (y - y1) * (y2 - y1);
        double AP_AC = (x - x1) * (x3 - x1) + (y - y1) * (y3 - y1);
        double u = (AB_AB * AP_AC - AB_AC * AP_AB) / (AC_AC * AB_AB - AB_AC * AB_AC);
        double v = (AC_AC * AP_AB - AB_AC * AP_AC) / (AC_AC * AB_AB - AB_AC * AB_AC);
        if (u >= 0 && v >= 0 && u + v <= 1) {
            return true;
        } else {
            return false;
        }
    }
public:
	Triangle() {}
	Triangle(double x1, double y1, double x2, double y2, double x3, double y3) : x1{x1}, y1{y1}, x2{x2}, y2{y2}, x3{x3}, y3{y3} {}
	Triangle(int x1, int y1, int x2, int y2, int x3, int y3) :
		x1{static_cast<double>(x1)},
		y1{static_cast<double>(y1)},
		x2{static_cast<double>(x2)},
		y2{static_cast<double>(y2)},
        x3{static_cast<double>(x3)},
        y3{static_cast<double>(y3)} {}
	double x1, y1, x2, y2, x3, y3;// 這是三角形的三個點左(x1, y1), 中(x2, y2), 右(x3, y3) 
};

#endif
