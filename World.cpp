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

			// círculo + círculo
			if (auto ca = dynamic_cast<CircleShape*>(a->shape.get())) {
				if (auto cb = dynamic_cast<CircleShape*>(b->shape.get())) {
					Vec2f dr = b->pos - a->pos;
					float dist = dr.len();
					float radiusSum = ca->radius + cb->radius;
					if (dist < radiusSum) {
						Contact c;
						c.a = a; c.b = b;
						c.normal = dist > 0 ? dr / dist : Vec2f(1, 0);
						c.penetration = radiusSum - dist;
						contacts.push_back(c);
					}
					continue;
				}
			}

			CircleShape* circleA = dynamic_cast<CircleShape*>(a->shape.get());
			RectangleShape* rectA = dynamic_cast<RectangleShape*>(a->shape.get());
			CircleShape* circleB = dynamic_cast<CircleShape*>(b->shape.get());
			RectangleShape* rectB = dynamic_cast<RectangleShape*>(b->shape.get());

			// círculo A vs rectángulo B
			if (circleA && rectB) {
				Vec2f rel = a->pos - b->pos;
				float cx = std::fmax(-rectB->halfWidth, std::fmin(rel.x, rectB->halfWidth));
				float cy = std::fmax(-rectB->halfHeight, std::fmin(rel.y, rectB->halfHeight));
				Vec2f closest(cx, cy);
				Vec2f diff = rel - closest;
				float distSq = diff.sqlen();
				if (distSq < circleA->radius * circleA->radius) {
					float dist = std::sqrt(distSq);
					Contact c;
					c.a = a; c.b = b;
					c.normal = dist > 0 ? -(diff / dist) : Vec2f(1, 0);
					c.penetration = circleA->radius - dist;
					contacts.push_back(c);
				}
				continue;
			}

			// círculo B vs rectángulo A
			if (circleB && rectA) {
				Vec2f rel = b->pos - a->pos;   // from rectangle A to circle B
				float cx = std::fmax(-rectA->halfWidth, std::fmin(rel.x, rectA->halfWidth));
				float cy = std::fmax(-rectA->halfHeight, std::fmin(rel.y, rectA->halfHeight));
				Vec2f closest(cx, cy);
				Vec2f diff = rel - closest;
				float distSq = diff.sqlen();
				if (distSq < circleB->radius * circleB->radius) {
					float dist = std::sqrt(distSq);
					Contact c;
					c.a = a; c.b = b;
					// Normal points from A (rectangle) to B (circle)
					c.normal = dist > 0 ? (diff / dist) : Vec2f(1, 0);
					c.penetration = circleB->radius - dist;
					contacts.push_back(c);
				}
				continue;
			}

			// rectángulo + rectángulo
			if (rectA && rectB) {
				Vec2f d = b->pos - a->pos;
				float overlapX = (rectA->halfWidth + rectB->halfWidth) - std::abs(d.x);
				float overlapY = (rectA->halfHeight + rectB->halfHeight) - std::abs(d.y);
				if (overlapX > 0 && overlapY > 0) {
					Contact c;
					c.a = a; c.b = b;
					if (overlapX < overlapY) {
						c.penetration = overlapX;
						c.normal = d.x > 0 ? Vec2f(1, 0) : Vec2f(-1, 0);
					} else {
						c.penetration = overlapY;
						c.normal = d.y > 0 ? Vec2f(0, 1) : Vec2f(0, -1);
					}
					contacts.push_back(c);
				}
				continue;
			}
		}
	}
}

void World::resolveCollisions() {
	for (Contact& contact : contacts) {
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
		if (body->invMass == 0) continue;
		// círculo
		if (auto circle = dynamic_cast<CircleShape*>(body->shape.get())) {
			float r = circle->radius;
			Vec2f& pos = body->pos;
			Vec2f& vel = body->vel;
			if (pos.x - r < left) { pos.x = left + r; vel.x = -vel.x * body->restitution; }
			if (pos.x + r > right) { pos.x = right - r; vel.x = -vel.x * body->restitution; }
			if (pos.y - r < top) { pos.y = top + r; vel.y = -vel.y * body->restitution; }
			if (pos.y + r > bottom) { pos.y = bottom - r; vel.y = -vel.y * body->restitution; }
		}

		// rectángulo
		else if (auto rect = dynamic_cast<RectangleShape*>(body->shape.get())) {
			Vec2f& pos = body->pos;
			Vec2f& vel = body->vel;
			float hw = rect->halfWidth;
			float hh = rect->halfHeight;
			if (pos.x - hw < left) { pos.x = left + hw; vel.x = -vel.x * body->restitution; }
			if (pos.x + hw > right) { pos.x = right - hw; vel.x = -vel.x * body->restitution; }
			if (pos.y - hh < top) { pos.y = top + hh; vel.y = -vel.y * body->restitution; }
			if (pos.y + hh > bottom) { pos.y = bottom - hh; vel.y = -vel.y * body->restitution; }
		}
	}
}