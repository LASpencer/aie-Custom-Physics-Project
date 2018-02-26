#include "SlugDemo.h"
#include "Input.h"

#include "Plane.h"

#include "Application2D.h"
#include "Slug.h"

const float SlugDemo::k_box_elasticity = 0.5f;
const float SlugDemo::k_box_friction = 0.2f;

const glm::vec2 SlugDemo::k_slug_start = {-40,-20};

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

	wall.setHeight(5);
	wall.setWidth(20);
	wall.setPosition({ 70, -22.5f });
	m_scene->addActor(wall.clone());

	wall.setHeight(39);
	wall.setPosition({ 70, 7.5f });
	m_scene->addActor(wall.clone());

	// TODO ramp and steps to reach higher level
}
