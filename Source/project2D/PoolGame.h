#pragma once
#include <queue>

#include "Sphere.h"
#include "Box.h"

#include "Demo.h"
#include "PoolPlayer.h"

enum EPoolTags : unsigned int {
	pocket = 1,
	ball = 2,
	cueball = 4,
	eightball = 8,
	solid_ball = 16,
	striped_ball = 32
};

enum EBallSuits {
	none,
	solid,
	striped,
	eight
};

enum EPoolGameStates {
	shot,		// Player taking a shot
	place_cue,	// Player replacing cue ball
	moving,		// Balls in motion
	game_over	// Game over
};

class PoolBall;
typedef std::shared_ptr<PoolBall> PoolBallPtr;
typedef std::weak_ptr<PoolBall> PoolBallWeakPtr;


class PoolGame : public Demo {
public:
	static const float k_table_width;
	static const float k_table_height;
	static const float k_rail_friction;
	static const float k_rail_elasticity;
	static const float k_rail_width;
	static const float k_stick_force_multiplier;
	static const float k_stick_max_force;
	static const float k_pocket_width;
	static const glm::vec4 k_felt_colour;
	static const glm::vec4 k_rail_colour;

	PoolGame();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	// Called to inform game that cue has hit a ball
	void OnCueHitBall(EBallSuits suit);

	// Informs game that a ball was sunk
	void OnBallSunk(PoolBall* ball);

	// Returns whether any balls of given suit are on the table
	bool AreAnySuitLeft(EBallSuits suit);

	PoolPlayer& currentPlayer();
	PoolPlayer& otherPlayer();

	int playerNumber();
	void nextPlayer();

protected:
	std::vector<PoolBallPtr> m_balls;
	std::vector<physics::BoxPtr> m_pockets;

	PoolPlayer m_player[2];
	size_t m_playerIndex;

	glm::vec2 m_cueContact;
	bool m_cueActive;
	bool m_break;		// Is this the break shot?
	EBallSuits m_firstHit;	// Suit of first ball hit by shot
	std::queue<PoolBall*> m_sunkThisRound;

	EPoolGameStates m_state;

	std::string m_message;

	virtual void setup();
	virtual void rack();

	void shootCue(physics::Sphere* cue, Application2D* app);

	bool isLegalCuePosition(glm::vec2 pos);
};