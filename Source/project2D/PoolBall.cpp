#include "PoolBall.h"

#include "PhysicsObject.h"

#include "PoolGame.h"

using namespace physics;

const glm::vec4 PoolBall::k_cue_colour = { 1,1,1,1 };
const glm::vec4 PoolBall::k_eight_colour = { 0.2f,0.2f,0.2f,1 };	// TODO make black when green bg added
const glm::vec4 PoolBall::k_striped_colour = { 1,1,0,1 };
const glm::vec4 PoolBall::k_solid_colour = {1,0,0,1};

const float PoolBall::k_radius = 2;	
const float PoolBall::k_mass = 1.6f;
const float PoolBall::k_cue_mass = 1.7f;		
const float PoolBall::k_elasticity = 1.f;
const float PoolBall::k_friction = 0.2f;
const float PoolBall::k_drag = 1.f;				
const float PoolBall::k_ang_drag = 0.5f;		
const float PoolBall::k_cue_drag = k_drag * k_cue_mass / k_mass;	// Scaled by mass, since drag is really friction
const float PoolBall::k_cue_ang_drag = k_ang_drag * k_cue_mass / k_mass;
const float PoolBall::k_min_speed = 0.1f;		
const float PoolBall::k_min_rotate = 0.1f;

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
		m_suit = EBallSuits::none;
	}
	else if (number <= 7) {
		// Solid colour
		colour = k_solid_colour;
		tags |= EPoolTags::solid_ball;
		m_suit = solid;
	}
	else if (number == 8) {
		// Eight ball
		colour = k_eight_colour;
		tags |= EPoolTags::eightball;
		m_suit = eight;
	}
	else {
		// Striped ball
		colour = k_striped_colour;
		tags |= EPoolTags::striped_ball;
		m_suit = striped;
	}
	// create sphere object
	m_sphere = SpherePtr(new Sphere({ 0,0 }, k_radius, { 0,0 }, 0,mass,k_elasticity,
									k_friction,drag,ang_drag,colour));
	m_sphere->setTags(tags);
}

bool PoolBall::isStopped()
{
	// If velocity less than min speed, ball is stopped
	glm::vec2 velocity = m_sphere->getVelocity();
	return !m_sphere->isAlive() || glm::dot(velocity, velocity) < k_min_speed * k_min_speed;
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
	// if under minimum speed/rotation bring to a stop
	glm::vec2 velocity = m_sphere->getVelocity();
	if (isStopped()) {
		m_sphere->setVelocity({ 0,0 });
		m_sphere->setAngularVelocity(0);	// Also stop spin when at rest
	}
	if (m_sphere->getAngularVelocity() < k_min_rotate) {
		m_sphere->setAngularVelocity(0);
	}
}

void PoolBall::OnCollision(physics::PhysicsObject * publisher, const physics::Collision & collision)
{
	PhysicsObject* other = collision.second;
	
	if (other->hasTags(EPoolTags::ball)) {
		if (m_number == 0) {
			// when cueball hits a ball, inform game 
			if (other->hasTags(EPoolTags::eightball)) {
				// inform game of eightball hit (might be foul)
				m_game->OnCueHitBall(eight);
			}
			else if (other->hasTags(EPoolTags::striped_ball)) {
				// inform game of striped ball hit (might be foul)
				m_game->OnCueHitBall(striped);
			}
			else if (other->hasTags(EPoolTags::solid_ball)) {
				// inform game of solid ball hit (might be foul)
				m_game->OnCueHitBall(solid);
			}
		}
		// TODO play cracking sound?
	}
	else if (other->hasTags(EPoolTags::pocket)) {
		// if hit pocket, sink ball
		m_sphere->kill();
		// inform game of ball sunk
		m_game->OnBallSunk(this);
	}
}
