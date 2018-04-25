#pragma once
#include <vector>
#include <chrono>
#include <random>
#include "Point.h"

#define PI 3.14159265

class Generator {

private:
	std::vector<Point> vertices;
	bool isConvex = true;
	int verticesCount;
	double radius;
	Point center;
	double toRadians(double degrees);
public:
	Generator() {}
	Generator(bool isConvex, int n, double radius, Point center = Point(0, 0)) {
		this->isConvex = isConvex;
		this->verticesCount = n;
		this->radius = radius;
		this->center = center;
	}

	int getVerticesCount();

	void inverseConvex();

	void setConvex(bool isConvex);

	void setVerticesCount(int n);

	void setRadius(double radius);

	void setCenter(double x, double y);

	void generatePolygon();

	std::vector<Point>& getVertices();

	Point getCenter();
};