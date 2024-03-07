#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
// our files
#include "../src/headers/Game.h"
#include "../src/headers/Board.h"
#include "../src/headers/Player.h"
#include "../src/headers/Constants.h"
#include "../src/headers/RandomNumberGeneration_helper.h"

using namespace std;

class GameImplementation {
private:
	int noOfRows;
	int noOfColumns;

	struct ShipInfo {
		ShipInfo(int arg_length, char arg_symbol, string arg_name);
		int length;
		char symbol;
		string name;
	};

	vector<ShipInfo*> Ships;

public:
	GameImplementation(int arg_noOfRows, int arg_noOfColumns);
	~GameImplementation(); // this is required since the implementation will be stored on the heap

	int rows() const;
	int columns() const;
	int noOfShips() const;
	int shipLength(int shipID) const;

	bool addShips(int length, char shipSymbol, string name);
	bool isValid(Coordinate coordinate) const;

	char shipSymbol(int shipID) const;

	void display() const;

	string shipName(int shipID) const;

	Coordinate randomCoordinate() const;

	Player* play(Player* player1, Player* player2, Board& board1, Board& board2, bool shouldPause);
};

GameImplementation::ShipInfo::ShipInfo(int arg_length, char arg_symbol, string arg_name) {
	length = arg_length;
	symbol = arg_symbol;
	name =  arg_name;
};

void waitForEnterKey()
{
    cout << "Press enter to continue: ";
    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
}

GameImplementation::GameImplementation(int arg_noOfRows, int arg_noOfColumns) {
	noOfRows = arg_noOfRows;
	noOfColumns = arg_noOfColumns;
}

GameImplementation::~GameImplementation() {
	// destructor loops through Ships to delete the ship at each pointer
	for (size_t i = 0, N = Ships.size(); i < N; ++i) {
		delete Ships[i];
	}
}

int GameImplementation::rows() const {
	return noOfRows;
}

int GameImplementation::columns() const {
	return noOfColumns;
}

int GameImplementation::noOfShips() const {
	return static_cast<int>(Ships.size());
}

int GameImplementation::shipLength(int shipID) const {
	return Ships[shipID]->length;
}

bool GameImplementation::addShips(int length, char shipSymbol, string name) {
	// creates the Ship on heap and adds a pointer to it to ships
	ShipInfo* shipInfo = new ShipInfo(length, shipSymbol, name);
	Ships.push_back(shipInfo);
	return true;
}

bool GameImplementation::isValid(Coordinate coordinate) const {
	return ( (coordinate.row >= 0) && (coordinate.row < rows() ) && (coordinate.column >= 0) && (coordinate.column < columns() ) );
}

char GameImplementation::shipSymbol(int shipID) const {
	return Ships[shipID]->symbol;
}

string GameImplementation::shipName(int shipID) const {
	return Ships[shipID]->name;
}

Coordinate GameImplementation::randomCoordinate() const {
	// generate random row and column coordinate within our bounds
	int randRowCoord = RandomNumberGeneration_helper::nextInt(rows() );
	int randColCoord = RandomNumberGeneration_helper::nextInt(columns() );

	return Coordinate(randRowCoord, randColCoord);
}

Player* GameImplementation::play(Player* player1, Player* player2, Board& board1, Board& board2, bool shouldPause) {
	// make sure all ships are placed in it's entirety
	cout << "Player may now place their battleships" << endl;

	bool shipsPlaced_Player1 = player1->placeShips(board1);
	bool shipsPlaced_Player2 = player2->placeShips(board2);

	if ( !(shipsPlaced_Player1 && shipsPlaced_Player2) ) {
		return nullptr;
	}

	cout << "All battleships have been placed, let the battle begin!" << endl << endl;

	//variables we want to change while attacking
	bool is_ShotHit = 0;
	bool is_ShipDestroyed = 0;
	bool is_WhoseTurn = 1;

	int  shipID = -1;

	unsigned int turn_counter = 0;

	// the game will end if one of the boards has all it's ships destroyed
	while ( !(board1.allShipsDestroyed() || board2.allShipsDestroyed() ) ) {

		if (is_WhoseTurn) {
			cout << player1->name() << "'s turn to attack" << endl;
			cout << player2->name() << "'s board before the attack: " << endl;

			// display player 2 board
			board2.display(player1->isHumanPlayer() );

			// player 1 recommends an attack
			Coordinate attackRecommended = player1->recommendAttack();
			// Player 1 attacks
			bool isFailureTest = board2.attack(attackRecommended, is_ShotHit, is_ShipDestroyed, shipID);

			while (!isFailureTest) {
				// player 1 records their failed attack
				player1->recordAttackResults(attackRecommended, 0, is_ShotHit, is_ShipDestroyed, shipID);
				// recommends attack again
				attackRecommended = player1->recommendAttack();
				// if previous attack was invalid
				isFailureTest = board2.attack(attackRecommended, is_ShotHit, is_ShipDestroyed, shipID);
			}

			cout << player2->name() << "'s board after the attack: " << endl;
			// display player 2 board
			board2.display(player1->isHumanPlayer() );
			// player 1 records their attack
			player1->recordAttackResults(attackRecommended, 1, is_ShotHit, is_ShipDestroyed, shipID);
			// player 2 records the attack
			player2->recordAttackByOpponent(attackRecommended);

			// switching turns
			is_WhoseTurn = 0;
			++turn_counter;

			// turns cannot possibly be more than the total no of cells in our grid.
			// break condition

			//** type cast for warning??
			/* if (turn_counter > static_cast<unsigned int>(nRows * nCols))
			*/
			if (turn_counter > noOfRows * noOfColumns) {
				return nullptr;
			}

			cout << endl;
		}

		else {
			cout << player2->name() << "'s turn to attack" << endl;
			cout << player1->name() << "'s board before the attack: " << endl;

			// display player 1 board
			board1.display(player2->isHumanPlayer() );

			// player 2 recommends an attack
			Coordinate attackRecommended = player2->recommendAttack();
			// player 2 attacks
			bool isFailureTest = board1.attack(attackRecommended, is_ShotHit, is_ShipDestroyed, shipID);

			while (!isFailureTest) {
				// player 2 records their failed attack
				player2->recordAttackResults(attackRecommended, 0, is_ShotHit, is_ShipDestroyed, shipID);
				// recommends attack again
				attackRecommended = player2->recommendAttack();
				// if previous attack was invalid
				isFailureTest = board1.attack(attackRecommended, is_ShotHit, is_ShipDestroyed, shipID);
			}

			cout << player1->name() << "'s board after the attack: " << endl;
			// display player 1 board
			board1.display(player2->isHumanPlayer() );
			// player 2 records their attack
			player2->recordAttackResults(attackRecommended, 1, is_ShotHit, is_ShipDestroyed, shipID);
			// player 1 records the attack
			player1->recordAttackByOpponent(attackRecommended);

			// switching turns
			is_WhoseTurn = 1;
			cout << endl;

		}
	}

	/*  Game Results:
	 *	announce winner
	 *	display winner's board - IF the loser is human
	 */
	Player* winner = nullptr;

	if (board1.allShipsDestroyed() ) {
		winner = player2;

		cout << player1->name() << " has no remaining ships. " << player2->name() <<" Wins in " << turn_counter << " turns."<< endl;

		if (player1->isHumanPlayer()) {
			board2.display(0);
		}
	}

	else {
		winner = player1;

		cout << player2->name() << " has no remaining ships. " << player1->name() <<" Wins in " << turn_counter << " turns."<< endl;

		if (player2->isHumanPlayer()) {
			board1.display(0);
		}
	}

    cout << endl;
    cout << "Game Over." << endl;
    cout << "Thanks for playing !" << endl;
    cout << "          - Charlie " << endl << endl;
    cout << "P.S. Remember to delete your players. "<< endl << endl << endl << endl;
    return winner;
}


//************************************************************************************
//************************* Game functions *******************************************
// These functions for the most part simply delegate to GameImplementation's functions.
// You don't need to really change any of the code from this point.
//************************************************************************************

Game::Game(int noOfRows, int noOfColumns) {

	if ( noOfRows < 1 || noOfRows > MAX_ROWS) {
		cout << "Number of rows must be >= 1 and <= " << MAX_ROWS << endl;
		exit(1);
	}

	if ( noOfColumns < 1 || noOfColumns > MAX_COLUMNS) {
		cout << "Number of rows must be >= 1 and <= " << MAX_COLUMNS << endl;
		exit(1);
	}

	p_gameImplementation = new GameImplementation(noOfRows, noOfColumns);
}

Game::~Game() {
	delete p_gameImplementation;
}

int Game::rows() const {
	return p_gameImplementation->rows();
}

int Game::columns() const {
	return p_gameImplementation->columns();
}

int Game::noOfShips() const {
	return p_gameImplementation->noOfShips();
}

int Game::shipLength(int shipID) const {
	assert(shipID >= 0 && shipID < noOfShips() );
	return p_gameImplementation->shipLength(shipID);
}

bool Game::addShips(int length, char ship_symbol, string name) {

	if (length < 1) {
		cout << "Bad ship length " << length << "; It must be >= 1" << endl;
		return false;
	}

	if (length > rows() && length > columns() ) {
        cout << "Bad ship length " << length << "; It won't fit on the board" << endl;
        return false;
	}

    if (!isascii(ship_symbol) || !isprint(ship_symbol))
    {
        cout << "Unprintable character with decimal value " << ship_symbol << " must not be used as a ship symbol" << endl;
        return false;
    }

	if (ship_symbol == 'X'  ||  ship_symbol == '.'  ||  ship_symbol == 'o' || ship_symbol == '-')
	{
		cout << "Character " << ship_symbol << " must not be used as a ship symbol" << endl;
		return false;
	}

	int total_Length = 0;

	for (int i = 0; i < noOfShips(); i++) {
		total_Length += shipLength(i);

		if (shipSymbol(i) == ship_symbol) {
			cout << "Ship symbol " << ship_symbol << " must not be used for more than one ship" << endl;
			return false;
		}
	}

	if ( (total_Length + length) > (rows() * columns() ) ) {
        cout << "Board is too small to fit all ships" << endl;
        return false;
	}

	return p_gameImplementation->addShips(length, ship_symbol, name);
}

bool Game::isValid(Coordinate coordinate) const {
	return p_gameImplementation->isValid(coordinate);
}

char Game::shipSymbol(int shipID) const {
	assert(shipID >= 0 && shipID < noOfShips() );
	return p_gameImplementation->shipSymbol(shipID);
}

string Game::shipName(int shipID) const {
	assert(shipID >= 0 && shipID < noOfShips() );
	return p_gameImplementation->shipName(shipID);
}

Coordinate Game::randomCoordinate() const {
	return p_gameImplementation->randomCoordinate();
}

Player* Game::play(Player* player1, Player* player2, bool shouldPause) {

	if (player1 == nullptr || player2 == nullptr || noOfShips() == 0) {
		return nullptr;
	}

	Board board1(*this);
	Board board2(*this);

	return p_gameImplementation->play(player1, player2, board1, board2, shouldPause);
}
