#include "World.h"
#include "Physics.h"


void World::tick(float dt) {
	for (auto& body : bodies) {
		if (body->invMass > 0) {
			body->applyForce(gravity * body->mass, dt);
		}
	}

	detectCollisions();
	resolveCollisions();
	handleBoundaryCollisions();

	for (auto& body : bodies) {
		body->update(dt);
	}
}

void World::detectCollisions() {
	contacts.clear();
	for (size_t i = 0; i < bodies.size(); ++i) {
		for (size_t j = i + 1; j < bodies.size(); ++j) {
			RigidBody* a = bodies[i].get();
			RigidBody* b = bodies[j].get();

			// solo círculos por ahora
			CircleShape* circleA = dynamic_cast<CircleShape*>(a->shape.get());
			CircleShape* circleB = dynamic_cast<CircleShape*>(b->shape.get());
			if (!circleA || !circleB) continue;

			Vec2f dr = b->pos - a->pos;
			float dist = dr.len();
			float radiusSum = circleA->radius + circleB->radius;

			if (dist < radiusSum) {  // hay colisión
				Contact c;
				c.a = a;
				c.b = b;
				c.normal = dist > 0 ? dr / dist : Vec2f(1, 0);
				c.penetration = radiusSum - dist;
				contacts.push_back(c);
			}
		}
	}
}

void World::resolveCollisions() {
	for (auto& contact : contacts) {
		RigidBody* a = contact.a;
		RigidBody* b = contact.b;

		// si ambos tienen masa infinita, no hacer nada
		if (a->invMass == 0 && b->invMass == 0) continue;

		Vec2f dv = b->vel - a->vel;
		float velAlongNormal = dv.dot(contact.normal);
		if (velAlongNormal > 0) continue;

		// coef. de restitución (rebote)
		float e = std::min(a->restitution, b->restitution);

		float impulseMagnitude = -(1 + e) * velAlongNormal;
		impulseMagnitude /= (a->invMass + b->invMass);

		Vec2f impulse = contact.normal * impulseMagnitude;
		a->vel -= impulse * a->invMass;
		b->vel += impulse * b->invMass;

		// corrección de posición para evitar superposición
		float percent = 0.2f;       // porcentaje de corrección por frame
		float slop = 0.01f;         // penetración permitida (evita temblores)
		float correction = std::max(contact.penetration - slop, 0.0f) * percent;
		Vec2f correctionV = contact.normal * correction;

		if (a->invMass > 0) a->pos -= correctionV * a->invMass;
		if (b->invMass > 0) b->pos += correctionV * b->invMass;
	}
}

void World::handleBoundaryCollisions() {
	for (auto& body : bodies) {
		if (body->invMass == 0) continue;  // solo cuerpos móviles
		auto circle = dynamic_cast<CircleShape*>(body->shape.get());
		if (!circle) continue;

		float r = circle->radius;
		Vec2f& pos = body->pos;
		Vec2f& vel = body->vel;

		// límite izq.
		if (pos.x - r < left) {
			pos.x = left + r;
			vel.x = -vel.x * body->restitution;
		}
		// límite dcho.
		if (pos.x + r > right) {
			pos.x = right - r;
			vel.x = -vel.x * body->restitution;
		}
		// límite superior
		if (pos.y - r < top) {
			pos.y = top + r;
			vel.y = -vel.y * body->restitution;
		}
		// límite inferior
		if (pos.y + r > bottom) {
			pos.y = bottom - r;
			vel.y = -vel.y * body->restitution;
		}
	}
}