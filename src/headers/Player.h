#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

using namespace std;

#include <string>

class Coordinate;
class Board;
class Game;

class Player {

public:
	Player(string name, const Game& game) : p_name(name), p_game(game) {}

	virtual ~Player() {};

	const Game& game() const {
		return p_game;
	}

	string name() const {
		return p_name;
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

private:
	string p_name;
	const Game& p_game;
};


Player* createPlayer(string playerType, string playerName, const Game& game);

#endif  // PLAYER_INCLUDED
