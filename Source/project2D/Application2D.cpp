#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"

#include "Sphere.h"
#include "Plane.h"
#include "Spring.h"
#include "Box.h"
#include "SoftBody.h"

using namespace physics;

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::startup() {
	
	m_2dRenderer = new aie::Renderer2D();
	m_2dRenderer->setCameraPos(0,0);

	aie::Gizmos::create(255u, 255u, 65535u, 65535u);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_cameraPos = { 0,0 };
	m_sceneExtent = 100;
	m_timer = 0;

	m_scene = new physics::PhysicsScene(0.01f, { 0, -5 });

	//TODO put objects in scene
	m_scene->addActor(new physics::Plane({ -1,0 }, 40,1,1));
	//m_scene->addActor(new physics::Plane({ -1,-1 }, 40,1,1));
	m_scene->addActor(new physics::Plane({ 1,0 }, 40,1,1));
	m_scene->addActor(new physics::Plane({ -1,1 }, 40, 1,1));
	m_scene->addActor(new physics::Plane({ 0,1 }, 40, 1,1));

	/*m_scene->addActor(new physics::Box({ 0.f,0 }, 5, 5, 0, {0,0 }, 0,1,0.5f));
	m_scene->addActor(new physics::Box({ 0,-5 }, 5, 5, 0, { 0,0 }, 0, 1, 0.5f));
	m_scene->addActor(new physics::Box({ 0.f,-10}, 5, 5, 0, { 0,0 }, 0, 1, 0.5f));*/

	//for (size_t i = 0; i < 1; ++i) {
	//	m_scene->addActor(new physics::Sphere({ 20,0 },3, { 0,-40 }, 0, 0.16f,1,0.5f));
	//	m_scene->addActor(new physics::Sphere({ -20,0 },3, { 10,0.5f }, 0, 0.17f,1,0.5f));
	//	m_scene->addActor(new physics::Sphere({ 20,-20 },3, { 0,0 }, 0, 0.17f,1,0.5f));
	//	m_scene->addActor(new physics::Box({ 0,10 }, 8, 3, 0.5f, { 10,-3 }, 0,1,0.5f));
	//	m_scene->addActor(new physics::Box({ 30,10 }, 5, 5, 0, { -2,-1 }, 5,1,0.5f));
	//}
	/*m_scene->addActor(new physics::Sphere({ -20,0 }, 3, { 10,0.5f }, 1, 0.17f, 1, 0.f, 0.3f, 0.1f, { 1,0,0,1 }));
	m_scene->addActor(new physics::Sphere({ -10,0 }, 3, { 10,0.5f }, 1, 0.17f, 1, 0.f, 0.0f, 0.1f, { 0,1,0,1 }));
	m_scene->addActor(new physics::Sphere({ 0,0 }, 3, { 10,0.5f }, 1, 0.17f, 1, 0.f, 0.3f, 0.0f, { 0,0,1,1 }));*/

	//SpherePtr s1(new Sphere({ 10,0 },3, { 0,0 }, 0, 1));
	//s1->setAngularVelocity(1);
	//m_scene->addActor(s1);

	//for (size_t i = 0; i < 1; ++i) {
	//	SpherePtr s2(new Sphere({ 5,-3 },3, { 0,0 },0, 1));
	//	SpringPtr spring(new Spring(100.f, 20,3.f, s1, s2, { 0,3 }, { 0,3 }));
	//	//s2->setAngularVelocity(-1);
	//	m_scene->addActor(s2);
	//	m_scene->addActor(spring);
	//}

	Box particle({ 0,0 }, 6,6, 0, { 0,0 }, 0, 1, 1, 0.1f, 0, 0, { 1,0,0,1 });

	SoftBody body({ -20,10 }, &particle, 10, 3, 6.f, 30.f, 30.f, 20.f, 1.f, { 1,1,1,1 });

	body.addToScene(m_scene);

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
		m_cameraPos.y += 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		m_cameraPos.y -= 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		m_cameraPos.x -= 100.0f * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		m_cameraPos.x += 100.0f * deltaTime;

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	aie::Gizmos::clear();

	glm::vec2 mousePos = screenToWorldSpace({(float)input->getMouseX(), (float)input->getMouseY()});


	aie::Gizmos::add2DCircle(mousePos, 1, 6, { 0.2f,.8f,.5f,1 });// Draw circle at mouse

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		m_scene->addActor(new Box(mousePos, 5, 5, 0, { 0,0 }, 0, 1, 0.5f,0.5f));
	}

	m_scene->update(deltaTime);
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();


	// begin drawing sprites
	m_2dRenderer->begin();

	// TODO extract stuff out into variables, make screen to world conversion functions
	float sceneHeight = m_sceneExtent * getWindowHeight() / getWindowWidth();
	aie::Gizmos::draw2D(glm::ortho<float>(m_cameraPos.x - m_sceneExtent, m_cameraPos.x + m_sceneExtent,
						m_cameraPos.y - sceneHeight, m_cameraPos.y + sceneHeight, -1.0f, 1.0f));
	
	
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

glm::vec2 Application2D::screenToWorldSpace(glm::vec2 pos)
{
	float scale =2.f * m_sceneExtent / (float)getWindowWidth();
	return (pos - (0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight()))) * scale + m_cameraPos;
}

glm::vec2 Application2D::worldToSceenSpace(glm::vec2 pos)
{
	float scale = 0.5f * (float)getWindowWidth() / m_sceneExtent;
	return scale * (pos - m_cameraPos) + 0.5f * glm::vec2((float)getWindowWidth(), (float)getWindowHeight());
}
