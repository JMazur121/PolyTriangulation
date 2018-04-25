#include "Generator.h"

double Generator::toRadians(double degrees)
{
	return (degrees * PI) / 180;
}

int Generator::getVerticesCount()
{
	return verticesCount;
}

void Generator::inverseConvex()
{
	this->isConvex = !isConvex;
}

void Generator::setConvex(bool isConvex)
{
	this->isConvex = isConvex;
}

void Generator::setVerticesCount(int n)
{
	this->verticesCount = n;
}

void Generator::setRadius(double radius)
{
	this->radius = radius;
}

void Generator::setCenter(double x, double y)
{
	center.x = x;
	center.y = y;
}

void Generator::generatePolygon()
{
	vertices.clear();
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	double d = radius;
	double phi = 360.0 / (double)verticesCount;
	std::uniform_real_distribution<double> randomDegree(0.0, phi);
	std::uniform_real_distribution<double> randomRadius(1.0, radius);
	for (int i = 0; i<verticesCount; ++i) {
		if (!isConvex)
			d = randomRadius(generator);
		double alpha = randomDegree(generator) + i*phi;
		alpha = toRadians(alpha);
		double x = center.x + cos(alpha)*d;
		double y = center.y + sin(alpha)*d;
		vertices.push_back(Point(x, y));
	}
}

std::vector<Point>& Generator::getVertices()
{
	return vertices;
}

Point Generator::getCenter()
{
	return center;
}
