#ifndef BOARD_INCLUDED
#define BOARD_INCLUDED

#include "Constants.h"

class Game;
class BoardImplementation;

class Board {

public:
	Board(const Game& game);
	~Board();

	void clearBoard();
	void blockSpacesOnBoard();
	void unblockSpacesOnBoard();
	void display(bool shotsOnly) const;

	bool placeShips(Coordinate top_left, int shipID, Direction direction);
	bool unplaceShips(Coordinate top_left, int shipID, Direction direction);
	bool attack(Coordinate coordinate, bool& shotHit, bool& shipsDestroyed, int& shipID);
	bool allShipsDestroyed() const;

	// prevent the Board object from being assigned or copied
	Board(const Board&) = delete;
	Board& operator=(const Board&) = delete;

private:
	BoardImplementation* p_boardImplementation;

};

#endif // BOARD_INCLUDED
