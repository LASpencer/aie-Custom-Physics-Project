#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"

#include "Sphere.h"
#include "Plane.h"
#include "Spring.h"
#include "Box.h"

using namespace physics;

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	aie::Gizmos::create(255u, 255u, 65535u, 65535u);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_cameraX = 0;
	m_cameraY = 0;
	m_timer = 0;

	m_scene = new physics::PhysicsScene(0.01f, { 0, -2 });

	//TODO put objects in scene
	m_scene->addActor(new physics::Plane({ -1,0 }, 40));
	m_scene->addActor(new physics::Plane({ -1,-1 }, 40));
	m_scene->addActor(new physics::Plane({ 1,0 }, 40));
	m_scene->addActor(new physics::Plane({ -1,1 }, 40, 0.8f));
	m_scene->addActor(new physics::Plane({ 0,1 }, 40,0.8f));

	for (size_t i = 0; i < 10; ++i) {
		//m_scene->addActor(new physics::Sphere({ 20,0 }, { 0,-40 }, 3, 0.16f));
		m_scene->addActor(new physics::Sphere({ -20,0 }, { 10,0.5f }, 3, 0.17f));
		m_scene->addActor(new physics::Sphere({ 20,-20 }, { 0,0 }, 3, 0.17f));
		m_scene->addActor(new physics::Box({ 0,10 }, 8, 3, 0.5f, { 10,-3 }, 0));
		m_scene->addActor(new physics::Box({ 30,10 }, 5, 5, 0, { -2,-1 }, 5));
	}

	//SpherePtr s1(new Sphere({ 20,0 }, { 0,0 }, 3, INFINITY));
	//s1->setAngularVelocity(1);
	//m_scene->addActor(s1);

	//for (size_t i = 0; i < 2; ++i) {
	//	SpherePtr s2(new Sphere({ -20,-10 }, { -26,-18 }, 3, 1));
	//	SpringPtr spring(new Spring(10, 30, 0.f, s1, s2));
	//	s2->setAngularVelocity(-1);
	//	m_scene->addActor(s2);
	//	m_scene->addActor(spring);
	//}

	return true;
}

void Application2D::shutdown() {
	
	delete m_font;
	delete m_2dRenderer;
}

void Application2D::update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	// use arrow keys to move camera
	if (input->isKeyDown(aie::INPUT_KEY_UP))
		m_cameraY += 500.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		m_cameraY -= 500.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		m_cameraX -= 500.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		m_cameraX += 500.0f * deltaTime;

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	aie::Gizmos::clear();

	m_scene->update(deltaTime);
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// begin drawing sprites
	m_2dRenderer->begin();

	// TODO extract stuff out into variables, make screen to world conversion functions
	static float aspectRatio = 16 / 9.f;
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));
	
	
	// output some text in yellow
	m_2dRenderer->setRenderColour(1, 1, 0, 1);
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	char energy[32];
	sprintf_s(energy, 32, "Energy: %f", m_scene->calculateEnergy());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, energy, 0, 720 - 64);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 96);

	// done drawing sprites
	m_2dRenderer->end();
}