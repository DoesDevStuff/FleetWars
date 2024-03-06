#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

using namespace std;

#include <string>

class Constants;
class Board;
class Game;

class Player {

private:
	string p_name;
	const Game& p_game;

public:
	Player(string name, const Game& game) {
		p_name = name;
		p_game = game;
	}

	virtual ~Player() {};

	string name() const {
		return p_name;
	}

	const Game& game() const {
		return p_game;
	}

	virtual bool isHumanPlayer() const {
		return false;
	}

	virtual bool placeShips(Board& board) = 0;

	virtual void recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID) = 0;
	virtual void recordAttackByOpponent(Coordinate coordinate) = 0;

	virtual Coordinate recommendAttack() = 0;

	// prevent the Player object from being assigned or copied
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
};


Player* createPlayer(string playerType, string playerName, const Game& game);

#endif  // PLAYER_INCLUDED
