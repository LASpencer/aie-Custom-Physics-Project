#pragma once

enum EBallSuits;

class PoolPlayer {
public:
	PoolPlayer();

	int getScore() { return m_score; }
	void addWin() { ++m_score; }
	void resetScore() { m_score = 0; }

	bool hasPenalty() { return m_penalty; }
	bool onFirstPenaltyShot() { return m_firstPenaltyShot; }
	void givePenalty();
	void useFirstShot();
	void resetPenalty();

	EBallSuits getSuit() { return m_suit; }
	void setSuit(EBallSuits suit) { m_suit = suit; }

	void startNewGame();

private:
	int m_score;
	bool m_penalty;
	bool m_firstPenaltyShot;
	EBallSuits m_suit;
};