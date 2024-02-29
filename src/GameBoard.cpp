/*
 * GameBoard.cpp
 *
 *  Created on: 25 Feb 2024
 *      Author: Charlie
 */

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "../src/headers/Utils.h"
#include "../src/headers/GameBoard.h"
#include "../src/headers/ships/AirCraft_Carrier.h"
#include "../src/headers/ships/Battleship.h"
#include "../src/headers/Constants.h"
#include "../src/headers/ships/Destroyer.h"
#include "../src/headers/ships/PatrolBoat.h"
#include "../src/headers/RandomNumberGeneration_helper.h"
#include "../src/headers/ships/Submarine.h"

using namespace std;

void GameBoard::addShips(int aircraftCarrierNum, int battleshipNum, int destroyerNum, int patrolboatNum, int submarineNum) {
	if (aircraftCarrierNum > 0) {
		setAircraftCarrierCount(aircraftCarrierNum);

		for(int i = 0 ; i < aircraftCarrierNum; i++) {
			p_pointerToShipObj.push_back(new AirCraft_Carrier());
			p_shipLookup.push_back(Utils::aircraft_carrier);
		}
	}

	if (battleshipNum > 0) {
		setBattleshipCount(battleshipNum);

		for(int i = 0 ; i < battleshipNum; i++) {
			p_pointerToShipObj.push_back(new Battleship());
			p_shipLookup.push_back(Utils::battleship);
		}
	}

	if (destroyerNum > 0) {
		setDestroyerCount(destroyerNum);

		for(int i = 0 ; i < destroyerNum; i++) {
			p_pointerToShipObj.push_back(new Destroyer());
			p_shipLookup.push_back(Utils::destroyer);
		}
	}

	if (patrolboatNum > 0) {
		setPatrolBoatCount(patrolboatNum);

		for(int i = 0 ; i < patrolboatNum; i++) {
			p_pointerToShipObj.push_back(new PatrolBoat());
			p_shipLookup.push_back(Utils::patrolboat);
		}
	}

	if (submarineNum > 0) {
		setSubmarineCount(submarineNum);

		for(int i = 0 ; i < submarineNum; i++) {
			p_pointerToShipObj.push_back(new Submarine());
			p_shipLookup.push_back(Utils::submarine);
		}
	}
}

void GameBoard::placeShip(Utils::SHIP_TYPE shipName, Utils::SHIP_ORIENTATION orient, int x, int y, char& shipCounter) {

	int shipDimensions = SHIP_SIZE[(int)shipName];
	char shipLetter = SHIP_REPRESENTING_LETTERS[(int)shipName];

	shipCounter ++;

	if (orient == Utils::horizontal) {
		for (int i = x; i < (x + shipDimensions); i++) {
			p_mainGameGrid[y][i] = shipLetter;
			p_shipLocation[y][i] = shipCounter;
		}
	}
	else if(orient == Utils::vertical) {
		for (int i = y; i < (y + shipDimensions); i++) {
			p_mainGameGrid[i][x] = shipLetter;
			p_shipLocation[i][x] = shipCounter;
		}
	}
}

void GameBoard::shipCheck(bool& isEmpty, int occupiedSpaces, bool isAI) {
	int flagCounter = 0;

	for(int i = 0; i < Constants::NUM_OF_SHIP_TYPES; i++) {

		if((occupiedSpaces - SHIP_SIZE[i]) <= 0 ) {
			flagCounter++;
		}
	}

	if(flagCounter == Constants::NUM_OF_SHIP_TYPES) {
		isEmpty = true;
	}

	if(isEmpty && !isAI) {
		cout << "Not enough space to add the ship. Please choose a smaller ship ! \n";
	}
}

void GameBoard::sinkShip(Utils::SHIP_TYPE sunkShipName, int sunkShip) {
	int Xcoord;
	int Ycoord;

	if(sunkShipName == Utils::aircraft_carrier) {
		setAircraftCarrierCount( (getAircraftCarrierCount() - 1 ) );
	}
	else if(sunkShipName == Utils::battleship) {
		setBattleshipCount( (getBattleshipCount() - 1) );
	}
	else if(sunkShipName == Utils::destroyer) {
		setDestroyerCount( (getDestroyerCount() - 1) );
	}
	else if(sunkShipName == Utils::patrolboat) {
		setPatrolBoatCount( (getPatrolBoatCount() - 1) );
	}
	else if(sunkShipName == Utils::submarine) {
		setSubmarineCount( (getSubmarineCount() - 1) );
	}

	for (int j = 0; j < static_cast<int>(p_shipLocation.size()); j++) {
		for (int i = 0; i < static_cast<int>(p_shipLocation.size()); i++) {
			if(getShipLocationCell(i, j) == static_cast<char>(sunkShip) ) {
				setBoardCell(i, j, '_');
			}
		}
	}

	for(int i = 1; i <static_cast<int>(p_lastHits.size()); i++) {
		Xcoord = getLastHitX(i);
		Ycoord = getLastHitY(i);

		if(getShipLocationCell(Xcoord, Ycoord) == '_') {
			eraseLatHit(i);
		}
	}
}

void GameBoard::placeExplosiveMines() {
	int x;
	int y;
	int mineToPlace;
	int maximumMines = 1;
	int coordinate;

	if (getGameDifficulty() == Utils::easy) {
		maximumMines = 1;
	}
	else if (getGameDifficulty() == Utils::medium) {
		maximumMines = 2;
	}
	else if (getGameDifficulty() == Utils::hard) {
		maximumMines = 3;
	}

	mineToPlace = RandomNumberGeneration_helper::nextInt(1, maximumMines);
	coordinate = RandomNumberGeneration_helper::nextInt(0, getBoardDifficultySize() - 1);

	for(int i = 0; i < mineToPlace; i++) {
		do {
			x = coordinate;
			y = coordinate;
		}
		while (getBoardCell(x, y) != '~');

		setBoardCell(x, y, '+');
	}
}

void GameBoard::explosiveMineDetonation(int x, int y, bool &hit) {
	setBoardCell(x, y, '#');
	// what radius are exploding in this case 2 spaces around
	for (int i = y - 2; i <= y + 2; i++) {
		for (int j = x - 2; j <= x + 2; j++) {
			if (i < 0 || j < 0 || i > getBoardDifficultySize() -1 || j > getBoardDifficultySize() - 1) {
				continue;
			}
			explosiveMineFiredAt(j, i, hit);
		}
	}
}

void GameBoard::explosiveMineFiredAt(int x, int y, bool &hit) {
	static bool explosionHit = false;
	hit = false;

	if(getBoardCell(x, y) == '~') {
		setBoardCell(x, y, 'X');
	}

	else if(isShip(x, y)) {
		hit = true;
		int damagedShip = (int)(getShipLocationCell(x, y) - 65);
		getShipObjCell(damagedShip)->shipIsHit();

		if(getShipObjCell(damagedShip)->hasShipSunk()) {
			Utils::SHIP_TYPE sunkenShip = getShipLookupCell(damagedShip);
			damagedShip = (int)(getShipLocationCell(x, y));
			setBoardCell(x, y, '_');
			sinkShip(sunkenShip, damagedShip);
		}

		else {
			setBoardCell(x, y, 'H');
			setLastHit(x, y);
		}
	}

	else if(getBoardCell(x, y) == '+') {
		explosiveMineDetonation(x, y, hit);
	}

	if(!explosionHit) {
		explosionHit = hit;
	}

	hit = explosionHit;
}

void GameBoard::printGameBoard() const {
	int difficultySize = getBoardDifficultySize();
	int vertNumber = difficultySize;
	int counter = 0;

	cout << "\n";

	for(int y = difficultySize - 1; y >= 0; y--) {

		for (int x = 0; x < difficultySize; x++) {

			if (counter == 0 && vertNumber < 10) {
				cout << " " << vertNumber;
			}
			else if (counter == 0 && vertNumber >= 10) {
				cout << vertNumber;
			}

			if(p_isShowingGameBoard) {
				cout << "  " << getBoardCell(x, y);
			}
			else {
				if (isShip(x, y) || getBoardCell(x, y) == '+') {
					cout << "  " << '~';
				}
				else {
					cout << "  " << getBoardCell(x, y);
				}
			}
			counter++;
		}
		cout << endl;
		counter = 0;
		vertNumber--;
	}

	cout << "    ";
	for (int i = 1; i < difficultySize + 1; i++) {
		if (i < 9) {
			cout << i << "  ";
		}
		else {
			cout << i << " ";
		}
	}

	cout << "\n\n";
}

bool GameBoard::checkShipPlacement(Utils::SHIP_TYPE shipName, Utils::SHIP_ORIENTATION orient, int x, int y, bool flagPlace) const {
	bool validSpot = true;
	int adjPlaceChance = RandomNumberGeneration_helper::nextInt(1, 100);
	int difficultySize = getBoardDifficultySize();
	int shipSize = SHIP_SIZE[(int)shipName];

	// check if this needs to be utils::vertical instead
	if (orient == Utils::horizontal) {

		if (x > (difficultySize - shipSize) ) {
			validSpot = false;
		}

		if (validSpot) {
			for (int i = x; i < x + shipSize; i++) {
				if (p_mainGameGrid[y][i] != '~') {
					validSpot = false;
				}
			}
		}
	}
	else if (orient == Utils::vertical) {

			if (y > (difficultySize - shipSize) ) {
				validSpot = false;
			}

			if (validSpot) {

				for (int i = y; i < y + shipSize; i++) {
					if (p_mainGameGrid[i][x] != '~') {
						validSpot = false;
					}
				}
			}
	}
	// 80% prob
	if (validSpot && flagPlace && adjPlaceChance < 80) {

		if (orient == Utils::horizontal) {

			for (int i = x; i < x + shipSize; i++) {

				if (i + 1 < difficultySize) {

					if (isShip(i + 1, y)) {
						validSpot = false;
						break;
					}
				}

				if (y + 1 < difficultySize) {

					if (isShip(i, y + 1)) {
						 validSpot = false;
						 break;
					 }
				}

				if( i - 1 > 0) {

					if (isShip(i - 1, y)) {
						validSpot = false;
						break;
					}
				}

				if (y - 1 > 0) {

					if (isShip(i, y - 1)) {
						validSpot = false;
						break;
					}
				}
			}
		}

		else if (orient == Utils::vertical) {

			for(int i = y; i < y + shipSize; i++) {

				if (i + 1 < difficultySize) {

					if (isShip(x, i + 1)) {
						validSpot = false;
						break;
					}
				}

				if (x + 1 < difficultySize) {

					if (isShip(x + 1, i)) {
						 validSpot = false;
						 break;
					 }
				}

				if( i - 1 > 0) {

					if (isShip(x, i - 1)) {
						validSpot = false;
						break;
					}
				}

				if (x - 1 > 0) {

					if (isShip(x - 1, i)) {
						validSpot = false;
						break;
					}
				}
			}
		}
	}

	return validSpot;
}

bool GameBoard::isShip(int x, int y) const {
	bool cellIsShip = false;

	for(int i = 0; i < Constants::NUM_OF_SHIP_TYPES; i++) {

		if(getBoardCell(x, y) == SHIP_REPRESENTING_LETTERS[i]) {
			cellIsShip = true;
			break;
		}
	}

	return cellIsShip;
}
