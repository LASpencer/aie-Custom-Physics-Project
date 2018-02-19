#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "PhysicsScene.h"

class Application2D : public aie::Application {
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	glm::vec2 screenToWorldSpace(glm::vec2 pos);

	glm::vec2 worldToSceenSpace(glm::vec2 pos);

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	physics::PhysicsScene*	m_scene;

	glm::vec2 m_cameraPos;

	float m_sceneExtent;
	float m_timer;
};