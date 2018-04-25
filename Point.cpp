#include "Point.h"

Point Point::operator+(const Point & p) const
{
	Point r;
	r.x = x + p.x;
	r.y = y + p.y;
	return r;
}

Point Point::operator-(const Point & p) const
{
	Point r;
	r.x = x - p.x;
	r.y = y - p.y;
	return r;
}

double Point::triangleArea(Point a, Point b, Point c)
{
	double val = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	val = fabs(val);
	return val / 2;
}

std::string Point::toString()
{
	std::string result;
	result = result + "(" + std::to_string(x) + "," + std::to_string(y) + ")";
	return result;
}

int Point::tripletOrientation(Point a, Point b, Point c)
{
	//z value of cross product
	//bc x ab; bc = [c.x - b.x ; c.y - b.y] ; ab = [b.x - a.x ; b.y - a.y]

	double zValue = (c.x - b.x)*(b.y - a.y) - (b.x - a.x)*(c.y - b.y);
	if (zValue == 0)
		return 0; //colinear
	return (zValue > 0) ? 1 : 2; // >0 --> clockwise; <0 --> counterclockwise
}

bool Point::liesOnSegment(Point p, Point q, Point r)
{
	if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x) && q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y))
		return true;
	return false;
}

bool Point::areIntersecting(Point p1, Point q1, Point p2, Point q2)
{
	// calculate four orientations
	int or1 = tripletOrientation(p1, q1, p2);
	int or2 = tripletOrientation(p1, q1, q2);
	int or3 = tripletOrientation(p2, q2, p1);
	int or4 = tripletOrientation(p2, q2, q1);

	//base case
	if ((or1 != or2) && (or3 != or4))
		return true;
	//special cases to check
	if ((or1 == 0) && liesOnSegment(p1, p2, q1))
		return true;
	if ((or2 == 0) && liesOnSegment(p1, q2, q1))
		return true;
	if ((or3 == 0) && liesOnSegment(p2, p1, q2))
		return true;
	if ((or4 == 0) && liesOnSegment(p2, q1, q2))
		return true;
	return false;
}
