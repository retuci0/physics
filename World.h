#pragma once

#include "RigidBody.h"
#include "Shapes.h"
#include "Vec2.h"
#include <vector>
#include <memory>

struct Contact {
	RigidBody* a;
	RigidBody* b;
	Vec2f normal;
	float penetration;
};


class World {
public:
	Vec2f gravity;
	float left, right, top, bottom;  // límites

	World(const Vec2f& g, float l, float r, float t, float b) 
		: gravity(g), left(l), right(r), top(t), bottom(b) {}

	void addBody(std::unique_ptr<RigidBody> body) {
		bodies.push_back(std::move(body));
	}

	const std::vector<std::unique_ptr<RigidBody>>& getBodies() const {
		return bodies;
	}

	void tick(float dt);

private:
	std::vector<std::unique_ptr<RigidBody>> bodies;
	std::vector<Contact> contacts;

	void detectCollisions();
	void resolveCollisions();
	void handleBoundaryCollisions();
};