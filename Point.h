#pragma once
#include <math.h>
#include <string>
#include <iostream>

struct Point {

public:
	double x, y;

	Point() {
		x = 0;
		y = 0;
	}

	Point(double x, double y) {
		this->x = x;
		this->y = y;
	}

	Point operator+ (const Point& p) const;

	Point operator- (const Point& p) const;

	//finds area using determinant of pair of vectors ab and ac
	static double triangleArea(Point a, Point b, Point c);

	std::string toString();

	//find orientation of triplet of points.
	//output: 0 - colinear, 1 - clockwise, 2 - counterclockwise
	static int tripletOrientation(Point a, Point b, Point c);

	//checks if point q lies on line segment pr; points p q r must be colinear
	static bool liesOnSegment(Point p, Point q, Point r);

	//checks if line segments p1q1 and p2q2 intersect
	static bool areIntersecting(Point p1, Point q1, Point p2, Point q2);
};