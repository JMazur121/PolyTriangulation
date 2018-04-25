#include "Polygon.h"

bool Polygon::isEmpty()
{
	return verticesList.empty();
}

bool Polygon::isTriangle(diagonalMatrix & dm, int i, int j, int k)
{
	//a triangle consists of 1 edge and 2 diagonals or 2 edges and 1 diagonal or 3 diagonals
	//if a line segment is neither edge nor diagonal, it can't be a part of a triangle
	bool edge;
	int absValue;

	absValue = abs(i - j);
	edge = ((absValue == 1) || (absValue == (verticesList.size() - 1))) ? true : false;
	if (!(edge || dm.get(i, j)))
		return false;

	absValue = abs(i - k);
	edge = ((absValue == 1) || (absValue == (verticesList.size() - 1))) ? true : false;
	if (!(edge || dm.get(i, k)))
		return false;

	absValue = abs(j - k);
	edge = ((absValue == 1) || (absValue == (verticesList.size() - 1))) ? true : false;
	if (!(edge || dm.get(j, k)))
		return false;

	return true;
}

double Polygon::cost(Point p1, Point p2, Point p3)
{
	double result = Point::triangleArea(p1, p2, p3);
	return result*result;
}

void Polygon::findTriangles(Pair ** table, int i, int j)
{
	if (j <= i + 1)
		return;
	Triangle tri;
	if (j == i + 2) {
		tri.p1 = i;
		tri.p2 = i + 1;
		tri.p3 = i + 2;
		triangulationList.push_back(tri);
		return;
	}
	tri.p1 = i;
	tri.p2 = table[i][j].k;
	tri.p3 = j;
	triangulationList.push_back(tri);
	findTriangles(table, i, table[i][j].k);
	findTriangles(table, table[i][j].k, j);
}

void Polygon::printTriangles(Pair ** table, int i, int j)
{
	if (j <= i + 1)
		return;
	if (j == i + 2) {
		std::cout << i << " " << i + 1 << " " << i + 2 << std::endl;
		return;
	}
	std::cout << i << " " << table[i][j].k << " " << j << std::endl;
	printTriangles(table, i, table[i][j].k);
	printTriangles(table, table[i][j].k, j);
}

double Polygon::convexNaiveTriangulation(int i, int j)
{
	if (j < i + 2)
		return 0;
	double current = DBL_MAX;
	for (int k = i + 1; k < j; ++k) {
		current = fmin(current, (convexNaiveTriangulation(i, k) + convexNaiveTriangulation(k, j) + cost(verticesList[i], verticesList[k], verticesList[j])));
	}
	return current;
}

double Polygon::concaveNaiveTriangulation(int i, int j, diagonalMatrix & dm)
{
	if (j < i + 2)
		return 0;
	if ((j < (verticesList.size() - 1)) && !(dm.get(i, j)))//ij should be a diagonal, except 0:n-1
		return MAX;
	double current = MAX;
	for (int k = i + 1; k < j; ++k) {
		if (!isTriangle(dm, i, k, j))
			continue;
		current = fmin(current, (concaveNaiveTriangulation(i, k, dm) + concaveNaiveTriangulation(k, j, dm) + cost(verticesList[i], verticesList[k], verticesList[j])));
	}
	return current;
}

double Polygon::convexTriangulation()
{
	// table to store results of subproblems. table[i][j] stores cost of triangulation of polygon P(Vi...Vj)
	// and vertex k where minimum cost was found.
	// table [0][j-1] contains final result
	int n = verticesList.size();
	Pair **table = new Pair*[n];
	for (int i = 0; i < n; ++i) {
		table[i] = new Pair[n];
	}

	for (int distance = 0; distance < n; distance++) {
		for (int i = 0, j = distance; j < n; i++, j++) {
			if (j < i + 2) // < 3 points
				table[i][j].val = 0.0;
			else { // >=3 points
				table[i][j].val = DBL_MAX;
				for (int k = i + 1; k < j; k++) {
					double val = table[i][k].val + table[k][j].val + cost(verticesList[i], verticesList[j], verticesList[k]);
					if (table[i][j].val > val) {
						table[i][j].val = val;
						table[i][j].k = k;
					}
				}
			}
		}
	}
	double result = table[0][n - 1].val;
	triangulationList.clear();
	findTriangles(table, 0, n - 1);
	for (int i = 0; i<n; i++)
		delete[] table[i];
	delete[] table;
	return result;
}

double Polygon::concaveTriangulation()
{
	int n = verticesList.size();
	Pair **table = new Pair*[n];
	for (int i = 0; i < n; ++i) {
		table[i] = new Pair[n]{};
	}

	findDiagonals();
	diagonalMatrix matrix(getDiagonals(), n);

	for (int distance = 0; distance < n; ++distance) {
		for (int i = 0, j = distance; j < n; ++i, ++j) {
			if (j < i + 2) // < 3 points
				table[i][j].val = 0.0;
			else if (j == i + 2) { // triangle
				if (matrix.get(i, j)) // ij is a diagonal
					table[i][j].val = cost(verticesList[i], verticesList[i + 1], verticesList[i + 2]);
				else
					table[i][j].val = MAX;
			}
			else { // >=3 points
				table[i][j].val = MAX;
				if ((j < (n - 1)) && !(matrix.get(i, j))) {//ij must be a diagonal, except 0:n-1
					continue;
				}
				for (int k = i + 1; k < j; ++k) {
					if (!isTriangle(matrix, i, j, k)) {
						continue;
					}
					double val = table[i][k].val + table[k][j].val + cost(verticesList[i], verticesList[j], verticesList[k]);
					if (table[i][j].val > val) {
						table[i][j].val = val;
						table[i][j].k = k;
					}
				}
			}
		}
	}

	double result = table[0][n - 1].val;
	triangulationList.clear();
	findTriangles(table, 0, n - 1);
	for (int i = 0; i<n; i++)
		delete[] table[i];
	delete[] table;
	return result;
}

void Polygon::printVertices()
{
	for (auto it = verticesList.begin(); it != verticesList.end(); ++it) {
		Point p = *it;
		std::cout << p.toString() << std::endl;
	}
	std::cout << std::endl;
}

bool Polygon::isConvex()
{
	bool sign = false;
	double x1, y1, x2, y2, z;
	int n = verticesList.size();
	for (int i = 0; i < n; ++i) {
		x1 = verticesList[(i + 2) % n].x - verticesList[(i + 1) % n].x;
		y1 = verticesList[(i + 2) % n].y - verticesList[(i + 1) % n].y;
		x2 = verticesList[i].x - verticesList[(i + 1) % n].x;
		y2 = verticesList[i].y - verticesList[(i + 1) % n].y;
		z = x1*y2 - x2*y1; //z coordinate of cross product
		if (i == 0)
			sign = (z > 0);
		else if (sign != (z > 0))
			return false;
	}
	return true;
}

std::vector<Point>& Polygon::getVertices()
{
	return verticesList;
}

std::vector<Triangle>& Polygon::getTriangles()
{
	return triangulationList;
}

std::vector<Diagonal>& Polygon::getDiagonals()
{
	return diagonalsList;
}

void Polygon::setVertices(const std::vector<Point>& pointsList)
{
	verticesList.clear();
	verticesList = pointsList;
}

double Polygon::optimalTriangulation()
{
	if (isConvex())
		return convexTriangulation();
	else
		return concaveTriangulation();
}

double Polygon::naiveTriangulation()
{
	if (isConvex())
		return convexNaiveTriangulation(0, verticesList.size() - 1);
	else {
		findDiagonals();
		diagonalMatrix matrix(getDiagonals(), verticesList.size());
		return concaveNaiveTriangulation(0, verticesList.size() - 1, matrix);
	}
}

bool Polygon::isDiagonal(int i, int j)
{
	if (abs(i - j) < 2)
		return false;
	if (i > j)
		std::swap(i, j);
	int n = verticesList.size();
	int begin1 = i + 1;
	int end1 = j - 1;
	int begin2 = (j + 1) % n;
	int end2 = (i > 0) ? i - 1 : n - 1;
	for (int k = begin1; k < end1; ++k) {
		if (Point::areIntersecting(verticesList[i], verticesList[j], verticesList[k], verticesList[k + 1]))
			return false;
	}
	for (int k = begin2; k != end2; k = (k + 1) % n) {
		if (Point::areIntersecting(verticesList[i], verticesList[j], verticesList[k], verticesList[(k + 1) % n]))
			return false;
	}
	//no intersections, check if line segment ij is completely inside or completely outside polygon
	// make vectors v1(i,i+1), v2(i,i-1), v3(i,j)

	Point v1 = verticesList[i + 1] - verticesList[i];
	Point v2 = (i > 0) ? (verticesList[i - 1] - verticesList[i]) : (verticesList[n - 1] - verticesList[i]);
	Point v3 = verticesList[j] - verticesList[i];
	//find cross products V1xV2, V1xV3 and V3xV2
	double crossProducts[3];
	crossProducts[0] = (v1.x * v2.y) - (v1.y * v2.x); // V1xV2
	crossProducts[1] = (v1.x * v3.y) - (v1.y * v3.x); // V1xV3
	crossProducts[2] = (v3.x * v2.y) - (v3.y * v2.x); // V3xV2

													  //angle between v1 and v2 (in counter clockwise direction) is <= 180
	if ((crossProducts[0] >= 0) && (crossProducts[1] >= 0) && (crossProducts[2] >= 0))
		return true;
	//angle between v1 and v2 (in counter clockwise direction) is > 180
	else if ((crossProducts[0] < 0) && ((crossProducts[1] >= 0) || (crossProducts[2] >= 0)))
		return true;
	else
		return false;
}

void Polygon::findDiagonals()
{
	diagonalsList.clear();
	int diagonals = verticesList.size() * (verticesList.size() - 3);
	diagonals = diagonals >> 1; // n(n-3)/2 for convex polygons
	int index = 0;
	int end;
	if (isConvex()) {
		while (diagonals > 0) {
			end = (index > 0) ? verticesList.size() - 1 : verticesList.size() - 2;
			for (int i = index + 2; i <= end; ++i) {
				diagonalsList.push_back(Diagonal(index, i));
				--diagonals;
			}
			++index;
		}
	}
	else {
		while (diagonals > 0) {
			end = (index > 0) ? verticesList.size() - 1 : verticesList.size() - 2;
			for (int i = index + 2; i <= end; ++i) {
				if (isDiagonal(index, i))
					diagonalsList.push_back(Diagonal(index, i));
				--diagonals;
			}
			++index;
		}
	}
}

