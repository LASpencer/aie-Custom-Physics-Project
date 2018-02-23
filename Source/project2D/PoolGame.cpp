#include "PoolGame.h"

using namespace physics;

PoolGame::PoolGame()
{
	m_scene = new PhysicsScene(0.01f, { 0,0 });
	setup();
}

void PoolGame::update(float deltaTime, Application2D* app)
{
	// TODO cuestick

	m_scene->update(deltaTime);
}

void PoolGame::draw(Application2D * app)
{
	// TODO draw
}

void PoolGame::setup()
{
	m_scene->addActor(new Plane({ 1,0 }, 80, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ -1,0 }, 80, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,1 }, 40, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,-1 }, 40, 1.0f, 0.0f, { 0,1,0,1 }));

	// TODO adding balls
	// TODO adding pockets

	// TODO need classes to control balls, pockets

	
}
