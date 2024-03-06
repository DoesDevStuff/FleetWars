#include "../src/headers/Board.h"
#include "../src/headers/Constants.h"
#include "../src/headers/Game.h"

#include <iostream>
#include <vector>

using namespace std;

bool search (char character, const vector<char>& vec) {
	for (size_t i = 0, vec_size = vec.size(); i < vec_size; ++i) {

		if (vec[i] == character) {
			return true;
		}
	}

	return false;
}


class BoardImplementation {
private:
  const Game& p_game;
  vector<char> p_board;

public:
	BoardImplementation(const Game& game);
	void clearBoard();
	void blockSpacesOnBoard();
	void unblockSpacesOnBoard();
	void display(bool shotsOnly) const;
	// functions used by possibilities_boards
	void update(Coordinate coordinate, char character);
	void read_to(vector<int>& data);

	bool placeShips(Coordinate top_left, int shipID, Direction direction);
	bool unplaceShips(Coordinate top_left, int shipID, Direction direction);
	bool attack(Coordinate coordinate, bool& shotHit, bool& shipsDestroyed, int& shipID);
	bool allShipsDestroyed() const;
};

BoardImplementation::BoardImplementation(const Game& game) {
	p_game = game;
	int rows = game.rows();
	int columns = game.columns();
	p_board.resize(rows * columns, '.');
}

void BoardImplementation::clearBoard() {
    for (size_t i = 0, board_size = p_board.size(); i < board_size; ++i) {
        p_board[i] = '.';
    }
}

void BoardImplementation::blockSpacesOnBoard() {
    for (size_t i = 0, board_size = p_board.size() / 2; i < board_size; ++i) {
        Coordinate Coordinate = p_game.randomCoordinate();
        p_board[p_game.columns() * Coordinate.row + Coordinate.column] = '-';
    }
}

void BoardImplementation::unblockSpacesOnBoard() {
    for (size_t i = 0, board_size = p_board.size(); i < board_size; ++i) {
        if (p_board[i] == '-') {p_board[i] = '.';};
    }
}

void BoardImplementation::display(bool shotsOnly) const {
    // shotsOnly determines whether the p_board will be displayed in full (false)
    // or only display the results of attacks (true)
    cout << "   ";
    size_t M_rows = p_game.rows();
    size_t N_columns = p_game.columns();

    for (size_t i = 0; i < N_columns; ++i ) {

    	cout << i << ' ';
    }
    cout << endl;

    for (size_t j = 0; j < M_rows; ++j ) {

        cout << j << ' ' << ' ';

        for (size_t k = 0; k < N_columns; ++k ) {

            if (shotsOnly && ( !(p_board[p_game.columns() * j + k] == 'o' || p_board[p_game.columns() * j + k] == '.' || p_board[p_game.columns() * j + k] == 'X' ) ) ) {

            	cout << '.' << ' ';
            }

            else {

                cout << p_board[p_game.columns() * j + k] << ' ';
            }
        }

        cout << endl;
    }
}

bool BoardImplementation::placeShips(Coordinate top_left, int shipID, Direction direction) {
	// invalid shipId
    if (shipID >= p_game.noOfShips() ) {
    	return false;
    }

    // ship already on the p_board
    if (search(p_game.shipSymbol(shipID), p_board)) {
    	return false;
    }

    size_t start = p_game.columns() * top_left.row + top_left.column;

    if (direction == horizontal) {

        // loop to check
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {

            // we are at the end of a row and have more to go
            if ( ( ( (start + i + 1) % p_game.columns() ) == 0) && (i+1 < shipLengthByID)) {
            	// ship goes off the end of row
            	return false;
            }

            if ( p_board[start + i] != '.') {
            	// ship overlaps over something
            	return false;
            }
        }

        // loop to change
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {

            p_board[start + i] = p_game.shipSymbol(shipID);
        }
    }

    else {
        // differences: indexing is more complicated, but going off of the end is easy to check
        // loop to check
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {

            if (start + p_game.columns() * i > p_board.size()) {
            	// ship goes off the end of column
            	return false;
            }

            if ( p_board[start + p_game.columns() * i] != '.') {
            	// ship overlaps over something
            	return false;
            }
        }

        // loop to change
        for(size_t i = 0, NshipLengthByID = p_game.shipLength(shipID); i < NshipLengthByID; ++i) {

            p_board[start + p_game.columns() * i] = p_game.shipSymbol(shipID);
        }
    }

    return true;
}

bool BoardImplementation::unplaceShips(Coordinate top_left, int shipID, Direction direction) {

	// invalid shipId
    if (shipID >= p_game.noOfShips() ) {
    	return false;
    }

    size_t start = p_game.columns() * top_left.row + top_left.column;

    // separate the checking and changing of loops so don't have to re-input in case of failure
    if (direction == horizontal) {

        // loop to check
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {
            if ( p_board[start + i] != p_game.shipSymbol(shipID)) {
            	// incomplete ship
            	return false;
            }
        }

        // loop to change
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {
        	p_board[start + i] = '.';
        }
    }

    else {

        // loop to check
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {

            if ( p_board[start + p_game.columns() * i] != p_game.shipSymbol(shipID)) {
            	return false;
            } // incomplete ship
        }

        // loop to change
        for(size_t i = 0, shipLengthByID = p_game.shipLength(shipID); i < shipLengthByID; ++i) {
        	p_board[start + p_game.columns() * i] = '.';
        }
    }

    return true;
}

bool BoardImplementation::attack(Coordinate coordinate, bool& shotHit, bool& shipsDestroyed, int& shipID) {

    if (coordinate.row >= p_game.rows() || coordinate.column >= p_game.columns() || coordinate.row < 0 || coordinate.column < 0) {
    	return false;
    }

    if (p_board[p_game.columns() * coordinate.row + coordinate.column] == 'o' || p_board[p_game.columns() * coordinate.row + coordinate.column] == 'X') {
    	return false;
    }

    if (p_board[p_game.columns() * coordinate.row + coordinate.column] != '.') {
        // if it hits an undamaged part of a ship
        cout << "The attack hit a ship!" << endl;

        shotHit = true;
        char hit_ship_symbol = p_board[p_game.columns() * coordinate.row + coordinate.column];

        p_board[p_game.columns() * coordinate.row + coordinate.column] = 'X';

        if (!search(hit_ship_symbol, p_board)) {
            // there are no more parts of the given ship that are undamaged (its character is gone)
            shipsDestroyed = true;

            // finding the correct shipId
            for (size_t i = 0, numOfShips = p_game.noOfShips(); i < numOfShips; ++i) {

                if (hit_ship_symbol == p_game.shipSymbol(static_cast<int>(i))) {

                    shipID = static_cast<int>(i);
                }
            }

            cout << "The attack sank the " << p_game.shipName(shipID) << "!" << endl;
        }

        else {

            shipsDestroyed = false;
        }
    }

    else {

        p_board[p_game.columns() * coordinate.row + coordinate.column] = 'o';
        shotHit = false;
    }
    return true;
}

bool BoardImplementation::allShipsDestroyed() const {

    for (size_t i = 0, boardSize = p_board.size(); i < boardSize; ++i) {
    	//if we find a character that isn't one of those 3, its from an undamaged segment of a ship
        if (!(p_board[i] == 'o' || p_board[i] == '.' || p_board[i] == 'X')) {
        	return false;
        }
    }
    return true;
}


//**************************************************************************************
//****************************** Board functions ***************************************
// These functions for the most part simply delegate to BoardImplementation's functions.
// You don't need to really change any of the code from this point.
//**************************************************************************************
Board::Board(const Game& game)
{
	p_boardImplementation = new BoardImplementation(game);
}

Board::~Board()
{
    delete p_boardImplementation;
}

void Board::clearBoard()
{
	p_boardImplementation->clearBoard();
}

void Board::blockSpacesOnBoard()
{
    return p_boardImplementation->blockSpacesOnBoard();
}

void Board::unblockSpacesOnBoard()
{
    return p_boardImplementation->unblockSpacesOnBoard();
}

void Board::display(bool shotsOnly) const {
	p_boardImplementation->display(shotsOnly);
}

bool Board::placeShips(Coordinate top_left, int shipID, Direction direction)
{
    return p_boardImplementation->placeShips(top_left, shipID, direction);
}

bool Board::unplaceShips(Coordinate top_left, int shipID, Direction direction)
{
    return p_boardImplementation->unplaceShips(top_left, shipID, direction);
}

bool Board::attack(Coordinate coordinate, bool& shotHit, bool& shipsDestroyed, int& shipID) {
    return p_boardImplementation->attack(coordinate, shotHit, shipsDestroyed, shipID);
}

bool Board::allShipsDestroyed() const
{
    return p_boardImplementation->allShipsDestroyed();
}
