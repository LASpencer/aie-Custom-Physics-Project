#pragma once

enum EBallSuits;

class PoolPlayer {
public:
	PoolPlayer();

	int getScore() { return m_score; }
	void addWin() { ++m_score; }
	void resetScore() { m_score = 0; }

	bool hasPenalty() { return m_penalty; }
	void addPenalty() { m_penalty = true; }
	void resetPenalty() { m_penalty = false; }

	EBallSuits getSuit() { return m_suit; }
	void setSuit(EBallSuits suit) { m_suit = suit; }

	void startNewGame();

private:
	int m_score;
	bool m_penalty;
	EBallSuits m_suit;
};