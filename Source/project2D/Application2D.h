#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "PhysicsScene.h"

class Demo;
class TitleScreen;
class PoolGame;
class SlugDemo;
class BouncingBallsDemo;
class RopeBridgeDemo;

class Application2D : public aie::Application {
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	glm::vec2 getCameraPos() { return m_cameraPos; }
	void setCameraPos(glm::vec2 pos) { m_cameraPos = pos; }

	glm::vec2 screenToWorldSpace(glm::vec2 pos);

	glm::vec2 worldToScreenSpace(glm::vec2 pos);

	inline float screenToWorldScale();

	inline float worldToScreenScale();

	aie::Renderer2D* getRenderer() { return m_2dRenderer; }

	aie::Font* getFont() { return m_font; }

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	Demo* m_currentDemo;

	TitleScreen* m_titleScreen;
	PoolGame* m_pool;
	SlugDemo* m_slugDemo;
	BouncingBallsDemo* m_ballDemo;
	RopeBridgeDemo* m_ropeDemo;

	glm::vec2 m_cameraPos;

	float m_sceneExtent;
	float m_timer;
};