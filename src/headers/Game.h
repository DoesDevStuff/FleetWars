#ifndef GAME_INCLUDED
#define GAME_INCLUDED

using namespace std;

#include <string>
#include <cassert>  // enforcing assertions when functions execute.

class Coordinate;
class Player;
class GameImplementation;

class Game {
private:
	GameImplementation* p_gameImplementation;

public:
	Game(int noOfRows, int noOfColumns);
	~Game();

	int rows() const;
	int columns() const;
	int noOfShips() const;
	int shipLength(int shipID) const;

	bool addShips(int length, char ship_symbol, string name);
	bool isValid(Coordinate coordinate) const;

	char shipSymbol(int shipID) const;

	string shipName(int shipID) const;

	Coordinate randomCoordinate() const;

	Player* play(Player* player1, Player* player2, bool shouldPause = true);

	// prevent the Game object from being assigned or copied
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
};

#endif // GAME_INCLUDED
