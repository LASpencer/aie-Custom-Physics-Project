#pragma once
#include "PhysicsObject.h"
#include "Sphere.h"
#include "ICollisionObserver.h"
#include "IFixedUpdater.h"

class PoolGame;

class PoolBall : public physics::ICollisionObserver, public physics::IFixedUpdater {
public:
	static const glm::vec4 k_cue_colour;
	static const glm::vec4 k_eight_colour;
	static const glm::vec4 k_stripe_colour;
	static const glm::vec4 k_solid_colour;

	PoolBall(int number, PoolGame* game);

	int getNumber() { return m_number; }
	bool isCue() { return m_number == 0; }

	bool isStopped();

	bool isSunk();

	physics::Sphere* getSphere() { return m_sphere.get(); }

	void place(glm::vec2 position, physics::PhysicsScene* scene);

	virtual void fixedUpdate(physics::PhysicsScene* scene);

	virtual void OnCollision(physics::PhysicsObject* publisher, const physics::Collision& collision);

private:
	int m_number;	// Number of ball, zero being cue
	physics::SpherePtr m_sphere;
	PoolGame* m_game;
};