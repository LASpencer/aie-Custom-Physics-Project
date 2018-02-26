#include "SlugDemo.h"
#include "Input.h"

#include "Plane.h"

#include "Application2D.h"
#include "Slug.h"

const float SlugDemo::k_box_elasticity = 0.5f;
const float SlugDemo::k_box_friction = 0.2f;

const glm::vec2 SlugDemo::k_slug_start = { -40,52 };//{-40,-20};

using namespace physics;

SlugDemo::SlugDemo() : m_scene(new PhysicsScene())
{
	setup();
}

void SlugDemo::update(float deltaTime, Application2D * app)
{
	aie::Input* input = aie::Input::getInstance();

	//middle mouse to center camera on slug
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_MIDDLE)) {
		app->setCameraPos(m_slug->getHead()->getPosition());
	}

	// TODO R to reset

	m_slug->update(deltaTime);

	m_scene->update(deltaTime);
}

void SlugDemo::draw(Application2D * app)
{
}

void SlugDemo::setup()
{
	m_scene->addActor(new Plane({ 0,1 }, 25));

	m_slug = std::make_shared<Slug>(k_slug_start);
	m_scene->addUpdater(m_slug);
	m_slug->addToScene(m_scene);

	// Add obstacles
	Box wall({ -10,1 }, 20, 40, 0, { 0,0 }, 0, INFINITY, k_box_elasticity, k_box_friction, 0, 0, { 0.8f,0.8f,0.2f,1 });
	wall.setStatic(true);
	m_scene->addActor(wall.clone());
	wall.setHeight(7);
	wall.setWidth(10);
	wall.setPosition({ 30, -21.5f });
	m_scene->addActor(wall.clone());

	wall.setHeight(4);
	wall.setWidth(20);
	wall.setPosition({ 70, -23.f });
	m_scene->addActor(wall.clone());

	wall.setHeight(40);
	wall.setPosition({ 80, 8.f });
	m_scene->addActor(wall.clone());

	wall.setHeight(2);
	wall.setPosition({ 100,-25 });
	wall.setStatic(false);
	wall.setAngularVelocity(-0.8f);
	m_scene->addActor(wall.clone());

	wall.setPosition({ 120,-15 });
	m_scene->addActor(wall.clone());

	wall.setPosition({ 120, 5 });
	m_scene->addActor(wall.clone());
	wall.setPosition({ 120, 25 });
	m_scene->addActor(wall.clone());

	wall.setPosition({ 100,-5 });
	wall.setAngularVelocity(0.8f);
	m_scene->addActor(wall.clone());

	wall.setPosition({ 100,15 });
	m_scene->addActor(wall.clone());
	wall.setPosition({ 100,35 });
	m_scene->addActor(wall.clone());

	wall.setStatic(true);
	wall.setWidth(45);
	wall.setOrientation(-0.5f);
	wall.setPosition({ 50,40 });
	m_scene->addActor(wall.clone());
	wall.setPosition({ 50,53 });
	m_scene->addActor(wall.clone());

	wall.setWidth(80);
	wall.setOrientation(0);
	wall.setPosition({ -10,50 });
	m_scene->addActor(wall.clone());

	wall.setWidth(67.5f);
	wall.setOrientation(0);
	wall.setPosition({ -3.75f,63 });
	m_scene->addActor(wall.clone());

	// Chimney
	// TODO figure out if doable, 
	wall.setWidth(4);
	wall.setHeight(180);
	wall.setPosition({ -50,73.5f });
	m_scene->addActor(wall.clone());

	wall.setHeight(100);
	wall.setPosition({ -30,110.f });
	m_scene->addActor(wall.clone());

	wall.setWidth(9);
	wall.setHeight(2);
	wall.setPosition({ -45, 75 });
	m_scene->addActor(wall.clone());

	wall.setPosition({ -45, 53 });
	m_scene->addActor(wall.clone());
	// TODO ramp and steps to reach higher level
}
