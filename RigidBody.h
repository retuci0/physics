#pragma once

#include "Vec2.h"
#include "Shape.h"
#include <memory>


class RigidBody {
public:
	Vec2f pos, vel;

	float rotation, angularVel;
	float mass, invMass;
	float restitution = 0.8f;

	std::unique_ptr<Shape> shape;

	RigidBody(const Vec2f& pos, float mass, std::unique_ptr<Shape> shape)
		: pos(pos), vel(0, 0), rotation(0), angularVel(0), mass(mass), shape(std::move(shape)) {
		if (mass > 0) invMass = 1.0f / mass;
		else invMass = 0;  // masa infinita (objeto estático)
	}

	void applyForce(const Vec2f& force, float dt) {
		vel += force * invMass * dt;  // F = m * a -> a = F / m
	}

	void update(float dt) {
		pos = pos + vel * dt;
		rotation += angularVel * dt;
	}
};