#include "Polygon.h"
#include "Generator.h"
#include "SFML\Graphics.hpp"
#include <stdlib.h>
#include <iostream>

void readInput(Generator& generator, int argc, char** argv) {
	bool isConvex = false;
	if ((argv[1][0] == 't') || (argv[1][0] == 'T')) {
		isConvex = true;
	}
	int n;
	double radius, x, y;
	n = atoi(argv[2]);
	radius = atof(argv[3]);
	generator.setConvex(isConvex);
	generator.setVerticesCount(n);
	generator.setRadius(radius);
	if (argc == 4) { //default center
		std::cout << std::endl;
		return;
	}
	x = atof(argv[4]);
	y = atof(argv[5]);
	generator.setCenter(x, y);
	std::cout << x << " " << y << std::endl;
}

void verticesToSFML(Polygon& polygon, sf::VertexArray& vertexArray) {
	vertexArray.clear();
	std::vector<Point> vect = polygon.getVertices();
	double minX = 0.0, maxX = 0.0, minY = 0.0, maxY = 0.0;
	for (auto it = vect.begin(); it != vect.end(); ++it) {
		Point p = *it;
		if (p.x > maxX)
			maxX = p.x;
		if (p.x < minX)
			minX = p.x;
		if (p.y > maxY)
			maxY = p.y;
		if (p.y < minY)
			minY = p.y;
	}
	double windowX = (maxX + minX) / 2;
	double windowY = (maxY + minY) / 2;
	double translateX = 683 - windowX; //1366/2
	double translateY = -384 - windowY; //-768/2
	for (auto it = vect.begin(); it != vect.end(); ++it) {
		Point p = *it;
		vertexArray.append(sf::Vertex(sf::Vector2f(p.x + translateX, -(p.y + translateY)), sf::Color::Black));
	}
	vertexArray.append(vertexArray[0]);
}

void drawLabels(sf::RenderWindow& window, sf::Font& font, sf::VertexArray& vertexArray) {
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::Red);
	for (auto i = 0; i<vertexArray.getVertexCount() - 1; ++i) {
		text.setString(std::to_string(i));
		text.setPosition(vertexArray[i].position);
		window.draw(text);
	}
}

void drawVerticesCount(sf::RenderWindow& window, sf::Font& font, Polygon& polygon) {
	sf::Text text;
	std::string t("Vertices: ");
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Red);
	t += std::to_string(polygon.getVertices().size());
	text.setString(t);
	text.setPosition(sf::Vector2f(0, 0));
	window.draw(text);
}

void drawConvex(sf::RenderWindow& window, sf::Font& font, Polygon& polygon) {
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Red);
	if (polygon.isConvex())
		text.setString("CONVEX");
	else
		text.setString("CONCAVE");
	text.setPosition(sf::Vector2f(0, 20));
	window.draw(text);
}

void prepareTriangulation(Polygon& polygon, sf::VertexArray& points, std::vector<sf::VertexArray>& triangles) {
	sf::VertexArray tris;
	tris.setPrimitiveType(sf::LineStrip);
	std::vector<Triangle> trisList = polygon.getTriangles();
	Triangle tri;
	for (int i = 0; i < trisList.size(); ++i) {
		tri = trisList[i];
		tris.clear();
		tris.append(points[tri.p1]);
		tris.append(points[tri.p2]);
		tris.append(points[tri.p3]);
		tris.append(points[tri.p1]);
		triangles.push_back(tris);
	}
}

void prepareDiagonals(Polygon& polygon, sf::VertexArray& points, std::vector<sf::VertexArray>& diagonals) {
	sf::VertexArray diagonal;
	diagonal.setPrimitiveType(sf::Lines);
	polygon.findDiagonals();
	std::vector<Diagonal> diagList = polygon.getDiagonals();
	for (int i = 0; i < diagList.size(); ++i) {
		diagonal.clear();
		diagonal.append(points[diagList[i].p1]);
		diagonal.append(points[diagList[i].p2]);
		diagonals.push_back(diagonal);
	}
}

void drawText(sf::RenderWindow& window, sf::Font& font, std::string& message, sf::Vector2f& position) {
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);
	text.setFillColor(sf::Color::Red);
	text.setString(message);
	text.setPosition(position);
	window.draw(text);
}

int main(int argc, char** argv) {
	if (argc < 4) {
		std::cout << "Wrong input" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (argc == 5) {
		std::cout << "Missing arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		std::cout << "Cannot load font from file" << std::endl;
		exit(EXIT_FAILURE);
	}

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(1366, 768), "Triangulation", sf::Style::Default, settings);
	sf::VertexArray lines;
	std::vector<sf::VertexArray> triangles;
	std::vector<sf::VertexArray> diagonals;
	lines.setPrimitiveType(sf::LineStrip);

	Generator gen;
	bool tri = false;
	bool diag = false;
	readInput(gen, argc, argv);
	gen.generatePolygon();
	Polygon polygon(gen.getVertices());
	verticesToSFML(polygon, lines);

	double optimalResult, naiveResult;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			tri = false;
			diag = false;
			gen.generatePolygon();
			polygon.setVertices(gen.getVertices());
			verticesToSFML(polygon, lines);
			sf::sleep(sf::milliseconds(300));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
			gen.inverseConvex();
			sf::sleep(sf::milliseconds(300));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			diag = true;
			diagonals.clear();
			prepareDiagonals(polygon, lines, diagonals);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
			optimalResult = polygon.optimalTriangulation();
			naiveResult = polygon.naiveTriangulation();
			triangles.clear();
			prepareTriangulation(polygon, lines, triangles);
			tri = true;
			diag = false;
			sf::sleep(sf::milliseconds(300));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			int n = gen.getVerticesCount();
			if (n>3)
				gen.setVerticesCount(n - 1);
			sf::sleep(sf::milliseconds(300));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			gen.setVerticesCount(gen.getVerticesCount() + 1);
			sf::sleep(sf::milliseconds(300));
		}
		window.clear(sf::Color::White);
		window.draw(lines);
		drawVerticesCount(window, font, polygon);
		drawConvex(window, font, polygon);
		drawLabels(window, font, lines);

		if (diag) {
			for (auto it = diagonals.begin(); it != diagonals.end(); ++it) {
				window.draw(*it);
			}
		}

		if (tri) {
			for (auto it = triangles.begin(); it != triangles.end(); ++it) {
				window.draw(*it);
			}
			std::string message = "Optimal result: " + std::to_string(optimalResult);
			drawText(window, font, message, sf::Vector2f(0, 40));
			message = "Naive result:   " + std::to_string(naiveResult);
			drawText(window, font, message, sf::Vector2f(0, 60));
		}
		window.display();
	}
	return 0;
}
