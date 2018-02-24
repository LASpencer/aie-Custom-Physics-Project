#include "PoolGame.h"
#include "Application2d.h"

using namespace physics;

const float PoolGame::k_table_width = 160;
const float PoolGame::k_table_height = 80;
const glm::vec4 PoolGame::k_felt_colour = { 0.f,1.f,0.f,1.f };

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
	glm::vec2 centre = app->worldToScreenSpace({ 0,0 });
	float scale = app->worldToScreenScale();
	app->getRenderer()->setRenderColour(k_felt_colour.r, k_felt_colour.g, k_felt_colour.b, k_felt_colour.a);
	app->getRenderer()->drawBox(centre.x, centre.y, scale * k_table_width, scale * k_table_height);
}

void PoolGame::setup()
{
	m_scene->addActor(new Plane({ 1,0 }, 0.5f * k_table_width, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ -1,0 }, 0.5f * k_table_width, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,1 }, 0.5f * k_table_height, 1.0f, 0.0f, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,-1 }, 0.5f * k_table_height, 1.0f, 0.0f, { 0,1,0,1 }));

	// TODO adding balls
	// TODO adding pockets

	// TODO need classes to control balls, pockets

	
}
