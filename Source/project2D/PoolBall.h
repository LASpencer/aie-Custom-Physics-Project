#pragma once
#include "PhysicsObject.h"
#include "Sphere.h"
#include "ICollisionObserver.h"
#include "IFixedUpdater.h"

class PoolGame;

class PoolBall;
typedef std::shared_ptr<PoolBall> PoolBallPtr;
typedef std::weak_ptr<PoolBall> PoolBallWeakPtr;

enum EBallSuits;

class PoolBall : public physics::ICollisionObserver, public physics::IFixedUpdater {
public:
	static const glm::vec4 k_cue_colour;
	static const glm::vec4 k_eight_colour;
	static const glm::vec4 k_striped_colour;
	static const glm::vec4 k_solid_colour;

	static const float k_radius;
	static const float k_mass;
	static const float k_cue_mass;
	static const float k_elasticity;
	static const float k_friction;
	static const float k_drag;				// Drag applied to ball from table
	static const float k_ang_drag;			// Angular drag
	static const float k_cue_drag;		
	static const float k_cue_ang_drag;
	static const float k_min_speed;			// Below this speed, ball comes to a stop
	static const float k_min_rotate;

	PoolBall(int number, PoolGame* game);

	int getNumber() { return m_number; }
	bool isCue() { return m_number == 0; }

	// Returns true if speed less than min_speed
	bool isStopped();

	// Returns true if ball was sunk
	bool isSunk();

	physics::Sphere* getSphere() { return m_sphere.get(); }

	EBallSuits getSuit() { return m_suit; }

	// Adds sphere to given scene, at set position
	void place(glm::vec2 position, physics::PhysicsScene* scene);

	virtual void fixedUpdate(physics::PhysicsScene* scene);

	virtual void OnCollision(physics::PhysicsObject* publisher, const physics::Collision& collision);

private:
	int m_number;					// Number of ball, zero being cue
	EBallSuits m_suit;
	physics::SpherePtr m_sphere;
	PoolGame* m_game;
};