#include "PoolBall.h"
#include "PoolGame.h"

using namespace physics;

const glm::vec4 PoolBall::k_cue_colour = { 1,1,1,1 };
const glm::vec4 PoolBall::k_eight_colour = { 0.3f,0.3f,0.3f,1 };	// TODO make black when green bg added
const glm::vec4 PoolBall::k_stripe_colour;
const glm::vec4 PoolBall::k_solid_colour;

PoolBall::PoolBall(int number, PoolGame* game) : m_number(number), m_game(game)
{
	// TODO create sphere object
	// TODO tag sphere appropriately
}

bool PoolBall::isStopped()
{
	//TODO
	return false;
}

bool PoolBall::isSunk()
{
	//TODO
	return false;
}

void PoolBall::place(glm::vec2 position, physics::PhysicsScene * scene)
{
	// TODO add self to scene
	// TODO add sphere to scene and set its position
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
