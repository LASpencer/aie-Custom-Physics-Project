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
	// TODO describe with comments
	static const float k_table_width;
	static const float k_table_height;
	static const float k_rail_friction;				// friction for sides of table
	static const float k_rail_elasticity;			// elasticity for sides of table
	static const float k_rail_width;				// thickness of sides of table
	static const float k_stick_force_multiplier;	// force per unit cue pulled back 
	static const float k_stick_max_force;			// Maximum force from cue stick
	static const float k_pocket_width;
	static const float k_cue_width;					// Pixels wide to render cue stick
	static const float k_score_margin;				// Score distance from right side of screen
	static const float k_turn_text_margin;			// Turn text distance from left of screen
	static const float k_message_margin;			// Message distance from left of screen
	static const glm::vec4 k_felt_colour;
	static const glm::vec4 k_rail_colour;
	static const glm::vec4 k_pocket_colour;
	static const glm::vec4 k_cornercap_colour;
	static const glm::vec4 k_score_colour;
	static const glm::vec4 k_message_colour;

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

	// Swaps current player
	void nextPlayer();

protected:
	physics::PhysicsScene* m_scene;

	std::vector<PoolBallPtr> m_balls;
	std::vector<physics::BoxPtr> m_pockets;

	PoolPlayer m_player[2];
	size_t m_playerIndex;

	glm::vec2 m_cueContact;		// Point where cue will strike
	bool m_cueActive;			// Is the cue being pulled back?
	bool m_break;				// Is this the break shot?
	EBallSuits m_firstHit;		// Suit of first ball hit by shot
	std::queue<PoolBall*> m_sunkThisRound; // All balls sunk this round

	EPoolGameStates m_state;	// Current game state

	std::string m_message;		// Message displayed at bottom of screen

	// Setup all objects in the game
	virtual void setup();

	// Place balls and start a new game of pool
	virtual void rack();		

	// If contact point is on cue ball, apply force to it from the cue stick
	void shootCue(physics::Sphere* cue, Application2D* app);

	// Returns true if cue ball could be placed at position
	bool isLegalCuePosition(glm::vec2 pos);

};