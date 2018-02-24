#include "PoolGame.h"

#include "Plane.h"
#include "Box.h"

#include "Application2d.h"
#include "PoolBall.h"


using namespace physics;

const float PoolGame::k_table_width = 160;
const float PoolGame::k_table_height = 80;
const float PoolGame::k_rail_friction = 0.1f;
const float PoolGame::k_rail_elasticity = 1.f;
const glm::vec4 PoolGame::k_felt_colour = { 0.f,1.f,0.f,1.f };

PoolGame::PoolGame() : m_balls(16,PoolBallPtr())
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
	//app->getRenderer()->drawBox(centre.x, centre.y, scale * k_table_width, scale * k_table_height,0,2);

	// TODO URGENT figure out how to get felt to render behind gizmos
}

void PoolGame::setup()
{
	m_scene->addActor(new Plane({ 1,0 }, 0.5f * k_table_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ -1,0 }, 0.5f * k_table_width, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,1 }, 0.5f * k_table_height, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	m_scene->addActor(new Plane({ 0,-1 }, 0.5f * k_table_height, k_rail_elasticity, k_rail_friction, { 0,1,0,1 }));
	
	// TODO maybe rails should be static boxes, with pockets behind them?

	// TODO adding pockets
	// TODO extract pocket dimensions out to constant
	// TODO pocket colour?
	Box pocket({ 0,0 }, 4, 4, 0);
	pocket.setStatic(true);
	pocket.setTags(EPoolTags::pocket);
	
	// Add each pocket
	pocket.setPosition({ 0, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ 0,-0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setOrientation(glm::quarter_pi<float>());
	pocket.setPosition({ -0.5f * k_table_width, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ 0.5f * k_table_width, -0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setOrientation(-glm::quarter_pi<float>());
	pocket.setPosition({ 0.5f * k_table_width, 0.5f * k_table_height });
	m_scene->addActor(pocket.clone());
	pocket.setPosition({ -0.5f * k_table_width, -0.5f * k_table_height });
	m_scene->addActor(pocket.clone());

	// create balls
	for (size_t i = 0; i < 16; ++i) {
		m_balls[i] = std::make_shared<PoolBall>(i, this);
		m_scene->addUpdater(std::static_pointer_cast<IFixedUpdater, PoolBall>(m_balls[i]));
	}
	// rack balls
	rack();
}

void PoolGame::rack()
{
	// TODO place each ball in starting position
	// TODO let cue be placed anywhere behind line?
	m_balls[0]->place({0.25f * k_table_width, 0}, m_scene);

	// First ball in triangle is at {-0.25f * k_table_width,0}
	// Next row is at 60 degrees from start, times 2*radius ( + epsilon?), next ball on row at {0,-2*radius + epsilon)
	const float sixtyDegrees = glm::third<float>() * glm::pi<float>();
	const float spacing = PoolBall::k_radius * 2.f + 0.001f;
	glm::vec2 diagonal = { -sinf(sixtyDegrees), cosf(sixtyDegrees) };
	glm::vec2 down = { 0, -1 };
	glm::vec2 footPosition = { -0.25f * k_table_width,0 };	// Point of triangle

	auto triangleSpot = [=](int row, int place) 
		{ return footPosition + row * spacing * diagonal + place * spacing * down; };

	m_balls[1]->place(triangleSpot(0, 0), m_scene);
	m_balls[2]->place(triangleSpot(1, 1), m_scene);
	m_balls[3]->place(triangleSpot(2, 0), m_scene);
	m_balls[4]->place(triangleSpot(3, 1), m_scene);
	m_balls[5]->place(triangleSpot(3, 3), m_scene);
	m_balls[6]->place(triangleSpot(4, 0), m_scene);
	m_balls[7]->place(triangleSpot(4, 3), m_scene);
	m_balls[8]->place(triangleSpot(2, 1), m_scene);
	m_balls[9]->place(triangleSpot(1, 0), m_scene);
	m_balls[10]->place(triangleSpot(2, 2), m_scene);
	m_balls[11]->place(triangleSpot(3, 0), m_scene);
	m_balls[12]->place(triangleSpot(3, 2), m_scene);
	m_balls[13]->place(triangleSpot(4, 1), m_scene);
	m_balls[14]->place(triangleSpot(4, 2), m_scene);
	m_balls[15]->place(triangleSpot(4, 4), m_scene);
}
