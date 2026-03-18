#pragma once

#include "Vec2.h"
#include <vector>
#include <string>

enum class ShapeType {
	CIRCLE,
	RECT
};

static const std::string getShapeName(ShapeType shape) {
	switch (shape) {
		case ShapeType::CIRCLE:
			return "bola";
			break;
		case ShapeType::RECT:
			return "cubo";
			break;
	}
}

class Shape {
public:
	virtual ~Shape() = default;
	virtual bool contains(const Vec2f& point, const Vec2f& pos) const = 0;
};

class CircleShape : public Shape {
public:
	float radius;
	CircleShape(float r) : radius(r) {}

	bool contains(const Vec2f& point, const Vec2f& pos) const override {
		return (point - pos).sqlen() <= radius * radius;
	}
};

class RectangleShape : public Shape {
public:
	float halfWidth, halfHeight;
	RectangleShape(float hw, float hh) : halfWidth(hw), halfHeight(hh) {}

	bool contains(const Vec2f& point, const Vec2f& pos) const override {
		return std::abs((point - pos).x) < halfWidth && std::abs((point - pos).y) < halfHeight;
	}
};
