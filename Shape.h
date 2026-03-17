#pragma once

#include "Vec2.h"
#include <vector>


class Shape {
public:
	virtual ~Shape() = default;
	virtual bool contains(const Vec2f& point) const = 0;
};

class CircleShape : public Shape {
public:
	float radius;
	CircleShape(float r) : radius(r) {}

	bool contains(const Vec2f& p) const override {
		return (p.sqlen()) <= radius * radius;
	}
};

class PolygonShape : public Shape {
public:
	std::vector<Vec2f> vertices;
};

