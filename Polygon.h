#pragma once
#define MAX 1e+100

#include "Point.h"
#include <vector>
#include <initializer_list>
#include <iostream>

struct Pair {
	double val;
	int k;

	Pair() {
		val = 0.0;
		k = 0;
	}

	Pair(double val, int k)
	{
		this->val = val;
		this->k = k;
	}
};

struct Triangle {
	int p1, p2, p3;
};

struct Diagonal {
	int p1;
	int p2;

	Diagonal(int p1, int p2) {
		this->p1 = p1;
		this->p2 = p2;
	}
};

struct diagonalMatrix {
	bool** matrix;
	int n;
public:
	diagonalMatrix(std::vector<Diagonal>& vect, int n) {
		matrix = new bool*[n];
		for (int i = 0; i < n; ++i) {
			matrix[i] = new bool[n] {};
		}
		this->n = n;
		for (auto it = vect.begin(); it != vect.end(); ++it) {
			matrix[(*it).p1][(*it).p2] = true;
		}
	}

	~diagonalMatrix() {
		for (int i = 0; i<n; i++)
			delete[] matrix[i];
		delete[] matrix;
	}

	bool get(int i, int j) {
		if (i > j)
			return matrix[j][i];
		return matrix[i][j];
	}
};

class Polygon {
private:
	std::vector<Point> verticesList;
	std::vector<Triangle> triangulationList;
	std::vector<Diagonal> diagonalsList;

	bool isEmpty();

	bool isTriangle(diagonalMatrix& dm, int i, int j, int k);

	double cost(Point p1, Point p2, Point p3);

	void findTriangles(Pair** table, int i, int j);

	void printTriangles(Pair** table, int i, int j);

	//this method returns only the final cost of triangulation
	double convexNaiveTriangulation(int i, int j);

	double concaveNaiveTriangulation(int i, int j, diagonalMatrix& dm);

	double convexTriangulation();

	double concaveTriangulation();

public:
	Polygon() {}

	Polygon(const std::vector<Point>& pointsList) : verticesList(pointsList) {}

	Polygon(std::initializer_list<Point> pointsList) {
		for (auto it = pointsList.begin(); it != pointsList.end(); ++it) {
			this->verticesList.push_back(*it);
		}
	}

	void printVertices();

	bool isConvex();

	std::vector<Point>& getVertices();

	std::vector<Triangle>& getTriangles();

	std::vector<Diagonal>& getDiagonals();

	void setVertices(const std::vector<Point>& pointsList);

	double optimalTriangulation();
	
	double naiveTriangulation();

	//1. Check intersections with all edges 2. Check if inside/outside polygon
	bool isDiagonal(int i, int j);

	void findDiagonals();
};