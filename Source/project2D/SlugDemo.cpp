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

	//middle mouse or right ctrl to center camera on slug
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_MIDDLE) || input->wasKeyPressed(aie::INPUT_KEY_RIGHT_CONTROL)) {
		app->setCameraPos(m_slug->getHead()->getPosition());
	}

	// R to reset
	if (input->wasKeyPressed(aie::INPUT_KEY_R))
	{
		// Reset camera
		app->setCameraPos({ 0,0 });
		// Reset
		m_scene->clear();
		m_slug.reset();
		setup();
	}

	m_slug->update(deltaTime);

	m_scene->update(deltaTime);

	// Draw slug's eyes over its head
	m_slug->drawEyes();
}

void SlugDemo::draw(Application2D * app)
{
	aie::Renderer2D* renderer = app->getRenderer();
	if (m_slug->hasWon()) {
		renderer->drawText(app->getFont(), "You ate the food!", 300, app->getWindowHeight() - 32);
	}
}

void SlugDemo::setup()
{
	m_scene->addActor(new Plane({ 0,1 }, 25));

	m_slug = std::make_shared<Slug>(k_slug_start);
	m_scene->addUpdater(m_slug);
	m_slug->addToScene(m_scene);
	m_slug->getHead()->addObserver(m_slug);

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

	// mangles

	wall.setHeight(2);
	wall.setWidth(15);
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

	wall.setWidth(60.f);
	wall.setOrientation(0);
	wall.setPosition({ 0,63 });
	m_scene->addActor(wall.clone());

	// Steps
	wall.setWidth(8);
	wall.setHeight(5);
	wall.setPosition({-45, 55});
	m_scene->addActor(wall.clone());

	wall.setWidth(10);
	wall.setPosition({ -55, 60 });
	m_scene->addActor(wall.clone());

	wall.setPosition({ -65, 65 });
	m_scene->addActor(wall.clone());

	wall.setPosition({ -75, 70 });
	m_scene->addActor(wall.clone());
	wall.setPosition({ -85, 75 });
	m_scene->addActor(wall.clone());

	// start wall
	wall.setWidth(4);
	wall.setHeight(85);
	wall.setPosition({ -50,17.5f });
	m_scene->addActor(wall.clone());

	// Win area
	wall.setWidth(30);
	wall.setHeight(4);
	wall.setPosition({ -105,78 });
	m_scene->addActor(wall.clone());

	wall.setWidth(4);
	wall.setHeight(30);
	wall.setPosition({ -120,95 });
	m_scene->addActor(wall.clone());

	// Goal

	Box goal({ -115,90 }, 6, 5, 0, { 0,0 }, 0, 0, 0, 0.1f, 0, 0, {0.1f,1,0.15f,1});
	goal.setStatic(true);
	goal.addTags(k_goal_tag);
	m_scene->addActor(goal.clone());

}
