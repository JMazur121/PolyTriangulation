#include "Generator.h"
#include "Polygon.h"

#include <iostream>
#include <string>

using namespace std::chrono;

void m1Test()
{
	Polygon polygon;
	high_resolution_clock::time_point begin, end;
	duration<double> time_span;
	std::vector<Point>points;
	double x, y, naiveResult, optimalResult;
	int nPoints;
	bool isConvex;
	while (!std::cin.eof())
	{
		points.clear();
		std::cin >> nPoints;
		for (int i = 0; i < nPoints; ++i)
		{
			std::cin >> x;
			std::cin >> y;
			points.push_back(Point(x, y));
		}
		polygon.setVertices(points);
		isConvex = polygon.isConvex();
		if (isConvex)
			std::cout <<"Convex\t" << "N = " << nPoints<< std::endl;
		else
			std::cout << "Concave\t" << "N = " << nPoints << std::endl;
		begin = high_resolution_clock::now();
		naiveResult = polygon.naiveTriangulation();
		end = high_resolution_clock::now();
		time_span = duration_cast<duration<double>>(end - begin);
		std::cout << "Naive took me:   " << time_span.count() << " seconds." << std::endl;

		begin = high_resolution_clock::now();
		optimalResult = polygon.optimalTriangulation();
		end = high_resolution_clock::now();
		time_span = duration_cast<duration<double>>(end - begin);
		std::cout << "Optimal took me: " << time_span.count() << " seconds." << std::endl;

		std::cout << "Naive result:    " << naiveResult << std::endl;
		std::cout << "Optimal result:  " << optimalResult << std::endl;
		std::cout << std::endl;
	}
}

void m2Test(int n, bool isConvex, double radius, bool optimal, int k=1, int step=0)
{
	Generator gen(isConvex, n, radius);
	Polygon poly;
	high_resolution_clock::time_point begin, end;
	duration<double> time_span;
	double result = 0.0;
	for (int i = 0; i < k; ++i)
	{
		gen.generatePolygon();
		poly.setVertices(gen.getVertices());
		std::cout << (poly.isConvex() ? "Convex\n" : "Concave\n");
		std::cout << "N = " << gen.getVerticesCount() << std::endl;
		if (optimal)
		{
			begin = high_resolution_clock::now();
			result = poly.optimalTriangulation();
			end = high_resolution_clock::now();
		}
		else
		{
			begin = high_resolution_clock::now();
			result = poly.naiveTriangulation();
			end = high_resolution_clock::now();
		}
		time_span = duration_cast<duration<double>>(end - begin);
		std::cout << (optimal ? "Optimal " : "Naive ");
		std::cout << "took me: " << time_span.count() << " seconds." << std::endl;
		std::cout << "Result : " << result << std::endl;
		std::cout << std::endl;
		gen.setVerticesCount(gen.getVerticesCount() + step);
		_sleep(10);
	}
}

duration<double> testLoop(int n, bool isConvex, bool optimal, int r)
{
	Generator gen(isConvex, n, 200.0);
	Polygon pol;
	high_resolution_clock::time_point begin, end;
	duration<double> time_span(0.0);
	for (int i = 0; i < r; ++i) {
		gen.generatePolygon();
		pol.setVertices(gen.getVertices());
		if (!isConvex)
		{
			while (pol.isConvex())
			{
				gen.generatePolygon();
				pol.setVertices(gen.getVertices());
			}
		}
		if (optimal)
		{
			begin = high_resolution_clock::now();
			pol.optimalTriangulation();
			end = high_resolution_clock::now();
		}
		else
		{
			begin = high_resolution_clock::now();
			pol.naiveTriangulation();
			end = high_resolution_clock::now();
		}
		time_span += duration_cast<duration<double>>(end - begin);
		_sleep(10);
	}
	time_span /= r;
	return time_span;
}

void m3Test(int n, bool isConvex, bool optimal, int k, int step, int r)
{
	std::vector<duration<double>>durations;
	std::vector<double>qn;
	duration<double> tmp, medianat;
	int nMediana, medianaPos, Tmediana, Tn, currentN = n;
	double numerator, denominator;
	medianaPos = k / 2;
	if (k % 2 == 1)
	{
		nMediana = n + medianaPos*step;
	}
	else
	{
		nMediana = ((n + medianaPos*step) + (n + ((medianaPos - 1)*step))) / 2;
	}
	medianat = testLoop(nMediana, isConvex, optimal, r);
	if (optimal)
		Tmediana = nMediana*nMediana*nMediana;
	else
		Tmediana = pow(3, nMediana - 2);
	for (int i = 0; i < k; ++i, currentN+=step)
	{
		tmp = testLoop(currentN, isConvex, optimal, r);
		durations.push_back(tmp);
		numerator = tmp.count() * Tmediana;
		Tn = (optimal)? currentN*currentN*currentN : pow(3, currentN - 2);
		denominator = Tn * medianat.count();
		qn.push_back(numerator / denominator);
	}
	std::cout << "Algorytm z asymptota O(";
	std::cout << (optimal ? "n^3)\n" : "3^(n-2))\n");
	std::cout << "n\t t(n)[ms]\t q(n)" << std::endl;
	if (k%2 == 1)
		for (int i = 0; i < k; ++i, n += step) {
			if (n == nMediana) {
				std::cout << "Nm" << "\t" << durations[i].count() << "\t" << qn[i] << std::endl;
				continue;
			}
			std::cout << n << "\t" << durations[i].count() << "\t" << qn[i] << std::endl;
		}
	else
	{
		for (int i = 0; i <= (medianaPos-1); ++i, n += step)
			std::cout << n << "\t" << durations[i].count() << "\t" << qn[i] << std::endl;
		std::cout << nMediana << "\t" << medianat.count() << "\t" << 1.0 << std::endl;
		for (int i = medianaPos;i<k;++i, n += step)
			std::cout << n << "\t" << durations[i].count() << "\t" << qn[i] << std::endl;
	}
}

void readInput(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Too few arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string str(argv[1]);
	if (str == "-h" || str == "-H")
	{
		std::cout << "Help" << std::endl;
		std::cout << "m1 : read the list of vertices from standard input and triangulate" << std::endl;
		std::cout << "syntax : -m1 < file.txt" << std::endl;
		std::cout << "m2 : generate random polygon and triangulate" << std::endl;
		std::cout << "syntax : -m2 -n -t||-f -r -o||-na <-k> <-s>" << std::endl;
		std::cout << "m3 : generate benchmarks" << std::endl;
		std::cout << "syntax: -m3 -n -t||-f -o||-na -k -s -r" << std::endl;
	}
	else if (str == "-m1")
	{
		m1Test();
	}
	else if (str == "-m2")
	{
		int n, k = 1, step = 0;
		bool isConvex, optimal;
		double radius;
		if (argc < 6)
		{
			std::cout << "Too few arguments" << std::endl;
			exit(EXIT_FAILURE);
		}
		n = atoi(&argv[2][2]);
		str = argv[3];
		isConvex = (str == "-t") ? true : false;
		radius = atof(&argv[4][2]);
		str = argv[5];
		optimal = (str == "-o") ? true : false;
		switch (argc)
		{
		case 8:
			k = atoi(&argv[6][2]);
			step = atoi(&argv[7][2]);
			break;
		case 7:
			k = atoi(&argv[6][2]);
			break;
		case 6:
		default:
			break;
		}
		m2Test(n, isConvex, radius, optimal, k, step);
	}
	else if (str == "-m3")
	{
		int n, k, step, r;
		bool isConvex, optimal;
		if (argc != 8)
		{
			std::cout << "Wrong number of arguments" << std::endl;
			exit(EXIT_FAILURE);
		}
		n = atoi(&argv[2][2]);
		str = argv[3];
		isConvex = (str == "-t") ? true : false;
		str = argv[4];
		optimal = (str == "-o") ? true : false;
		k = atoi(&argv[5][2]);
		step = atoi(&argv[6][2]);
		r = atoi(&argv[7][2]);
		m3Test(n, isConvex, optimal, k, step, r);
	}
	else
		std::cout << "Unknown command" << std::endl;
}

int main(int argc, char** argv) {
	readInput(argc, argv);
	return 0;
}