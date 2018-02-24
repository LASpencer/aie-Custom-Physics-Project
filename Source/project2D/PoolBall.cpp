#include "PoolBall.h"

#include "PhysicsObject.h"

#include "PoolGame.h"

using namespace physics;

const glm::vec4 PoolBall::k_cue_colour = { 1,1,1,1 };
const glm::vec4 PoolBall::k_eight_colour = { 0.2f,0.2f,0.2f,1 };	// TODO make black when green bg added
const glm::vec4 PoolBall::k_striped_colour = { 1,1,0,1 };
const glm::vec4 PoolBall::k_solid_colour = {1,0,0,1};

const float PoolBall::k_radius = 2;		//TODO figure out 
const float PoolBall::k_mass = 1.6f;
const float PoolBall::k_cue_mass = 1.7f;
const float PoolBall::k_elasticity = 1.f;
const float PoolBall::k_friction = 0.1f;
const float PoolBall::k_drag = 0.1f;
const float PoolBall::k_ang_drag = 0.01f;
const float PoolBall::k_cue_drag = k_drag * k_cue_mass / k_mass;
const float PoolBall::k_cue_ang_drag = k_ang_drag * k_cue_mass / k_mass;

PoolBall::PoolBall(int number, PoolGame* game) : m_number(number), m_game(game)
{
	float mass = k_mass;
	float drag = k_drag;
	float ang_drag = k_ang_drag;
	glm::vec4 colour;
	unsigned int tags = EPoolTags::ball;
	// Determine type of ball

	if (number == 0) {
		// Cue ball
		mass = k_cue_mass;
		drag = k_cue_drag;
		ang_drag = k_cue_ang_drag;
		colour = k_cue_colour;
		tags |= EPoolTags::cueball;
	}
	else if (number <= 7) {
		// Solid colour
		colour = k_solid_colour;
		tags |= EPoolTags::solid_ball;
	}
	else if (number == 8) {
		// Eight ball
		colour = k_eight_colour;
		tags |= EPoolTags::eightball;
	}
	else {
		// Striped ball
		colour = k_striped_colour;
		tags |= EPoolTags::striped_ball;
	}
	// create sphere object
	m_sphere = SpherePtr(new Sphere({ 0,0 }, k_radius, { 0,0 }, 0,mass,k_elasticity,
									k_friction,drag,ang_drag,colour));
	m_sphere->setTags(tags);
}

bool PoolBall::isStopped()
{
	//TODO
	return false;
}

bool PoolBall::isSunk()
{
	// If sphere was killed by falling into pocket, ball counts as sunk
	return !(m_sphere->isAlive());
}

void PoolBall::place(glm::vec2 position, physics::PhysicsScene * scene)
{
	// Reset as alive if killed, and place at rest in given position
	m_sphere->resetAlive();
	m_sphere->setPosition(position);
	m_sphere->setVelocity({ 0,0 });
	m_sphere->setAngularVelocity(0);
	// Add sphere to scene
	scene->addActor(std::static_pointer_cast<PhysicsObject, Sphere>(m_sphere));
	// TODO maybe enable_shared_from_this and make responsible for adding self to scene?
}

void PoolBall::fixedUpdate(physics::PhysicsScene * scene)
{
	// TODO if under minimum speed/rotation bring to a stop
}

void PoolBall::OnCollision(physics::PhysicsObject * publisher, const physics::Collision & collision)
{
	// TODO if cueball hits a ball, inform game (for miss/off colour/eight ball foul)
	// TODO if hit pocket, sink ball
}
