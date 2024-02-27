/*
 * PlayerBoard.cpp
 *
 *  Created on: 25 Feb 2024
 *      Author: Charlie
 */

#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <functional>
#include <vector>
#include "../src/headers/Utils.h"
#include "../src/headers/GameBoard.h"
#include "../src/headers/PlayerBoard.h"
#include "../src/headers/Constants.h"
#include "../src/headers/AI_Board.h"

using namespace std;

void PlayerBoard::playerChooseShips() {
	int totalOccupiedSpacesOnBoard = getTotalOccupiedSpacesOnBoard();
	int shipChoice;
	int aircraftCarrierCount = 0;
	int battleshipCount = 0;
	int destroyerCount = 0;
	int patrolboatCount = 0;
	int submarineCount = 0;
	int currentShipFleet = 0;

	bool isOccupiedSpacesEmpty = false;

	reference_wrapper<int> playerShipFleet[5] = {aircraftCarrierCount, battleshipCount, destroyerCount, patrolboatCount, submarineCount};

	Utils::clearScreen();

	while(!isOccupiedSpacesEmpty) {
		// here we check for the size of the smallest boat which is of space of 2.
		// This ensures that larger space occupying ships aren't assigned if there isn't any space left
		// and obviously that at least there need to space for the smallest boat to fit
		if ( (totalOccupiedSpacesOnBoard - currentShipFleet) >= 2) {
			cout << "Choose your ships for your battle fleet:\n"
					<< "[1] Aircraft Carriers: " << Constants::AIRCRAFT_CARRIER_SIZE << " board spaces" << endl
					<< "[2] Battleships: " << Constants::BATTLESHIP_SIZE << " board spaces" << endl
					<< "[3] Destroyers: " << Constants::DESTROYER_SIZE << " board spaces" << endl
					<< "[4] PatrolBoats: " << Constants::PATROLBOAT_SIZE << " board spaces" << endl
					<< "[5] Submarines: " << Constants::SUBMARINE_SIZE << " board spaces" << endl
					<< "\nYou have " << (totalOccupiedSpacesOnBoard - currentShipFleet) << " board spaces left." << "\n";
			cin >> shipChoice;

			while(cin.fail() || shipChoice < 1 || shipChoice > 5) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "That is not a valid option. " << endl
						<< "Please choose a ship for your battle fleet: ";
				cin >> shipChoice;
			}

			shipChoice--;

			if (totalOccupiedSpacesOnBoard - (currentShipFleet + SHIP_SIZE[shipChoice]) >= 0) {
				playerShipFleet[shipChoice]++;
				currentShipFleet += SHIP_SIZE[shipChoice];
				Utils::clearScreen();
			}

			else {
				Utils::clearScreen();
				shipCheck(isOccupiedSpacesEmpty, totalOccupiedSpacesOnBoard, false);
			}
		}

		else {
			isOccupiedSpacesEmpty = true;
		}
	}

	addShips(aircraftCarrierCount, battleshipCount, destroyerCount, patrolboatCount, submarineCount);
}

void PlayerBoard::playerPlaceShips() {
	int x;
	int y;
	int userOrient;
	int difficultySize = getBoardDifficultySize();

	string shipType;
	bool badPlacement = false;
	char shipCounter = 64;

	Utils::SHIP_ORIENTATION orientation;

	for(int i = 0; i < getShipLookupSize(); i++) {
		do {
			if(!badPlacement) {
				Utils::clearScreen();
			}
			badPlacement = false;

			Utils::printCenter("Place your ships:\n", difficultySize);
			printGameBoard();

			if (getShipLookupCell(i) == Utils::aircraft_carrier) {
				shipType = "aircraft_carrier";
			}
			else if (getShipLookupCell(i) == Utils::battleship) {
				shipType = "battleship";
			}
			else if (getShipLookupCell(i) == Utils::destroyer) {
				shipType = "destroyer";
			}
			else if (getShipLookupCell(i) == Utils::patrolboat) {
				shipType = "patrolBoat";
			}
			else {
				shipType = "submarine";
			}

			cout << "What orientation would you like your " << shipType << " of size "
					<< SHIP_SIZE[getShipLookupCell(i)] << " to be in ?" << endl
			<< "[1]\t Horizontal\n[2]\t Vertical\n: ";

			cin >> userOrient;

			if( (userOrient != 1 && userOrient != 2) || cin.fail() ) {

				while ((userOrient != 1 && userOrient != 2) || cin.fail() ) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid orientation entered. Please enter again.\n";
					cin >> userOrient;
				}
			}

			if (userOrient == 1) {
				orientation = Utils::horizontal;
			}
			else if (userOrient == 2) {
				orientation = Utils::vertical;
			}

			cout << "Where would you like to place your " << shipType << "?\nX Coordinate: ";
			cin >> x;

			if( x > getBoardDifficultySize() || x < 1 || cin.fail() ) {
				while (x > getBoardDifficultySize() || x < 1 || cin.fail() ) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid coordinate entered. Please enter again.\n";
					cin >> x;
				}
			}

			cout << "Y coordinate";
			cin >> y;

			if( y > getBoardDifficultySize() || y < 1 || cin.fail() ) {
				while (y > getBoardDifficultySize() || y < 1 || cin.fail() ) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid coordinate entered. Please enter again.\n";
					cin >> y;
				}
			}

			x--;
			y--;

			if ( checkShipPlacement(getShipLookupCell(i), orientation, x, y, false) ) {
				placeShip(getShipLookupCell(i), orientation, x, y, shipCounter);
				printGameBoard();
			}

			else {
				badPlacement = true;
				Utils::clearScreen();
				cout << "Invalid ship position entered. Please pick a new one.\n";
			}
		}
		while (badPlacement);
	}
}

void PlayerBoard::playerFire(int& x, int& y) {
	cout << "Enter coordinates to fire upon\nX Coordinate: ";
	cin >> x;

	if( x > getBoardDifficultySize() || x < 1 || cin.fail() ) {
		while (x > getBoardDifficultySize() || x < 1 || cin.fail() ) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid coordinate entered. Please enter again.\n";
			cin >> x;
		}
	}

	cout << "Y coordinate";
	cin >> y;

	if( y > getBoardDifficultySize() || y < 1 || cin.fail() ) {
		while (y > getBoardDifficultySize() || y < 1 || cin.fail() ) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid coordinate entered. Please enter again.\n";
			cin >> y;
		}
	}

	x--;
	y--;
}

void PlayerBoard::fire(int x, int y, bool& hit) {
	hit = false;
	int difficultySize = getBoardDifficultySize();
	Utils::clearScreen();

	cout << "AI's Turn\n";

	if (getBoardCell(x, y) == '~') {
		setBoardCell(x, y, 'X');
		Utils::printCenter("MISS", difficultySize);
	}

	else if (isShip(x, y) ) {
		hit = true;
		int damagedship = (int)(getShipLocationCell(x, y) - 65);
		getShipObjCell(damagedship)->shipIsHit();

		if ( getShipObjCell(damagedship)->hasShipSunk() ) {
			Utils::SHIP_TYPE sunkenship = getShipLookupCell(damagedship);
			damagedship = (int)getShipLocationCell(x, y);
			setBoardCell(x, y, '_');
			sinkShip(sunkenship, damagedship);
			Utils::printCenter("Ship has Sunk!!", difficultySize);
		}
		else {
			setBoardCell(x, y, 'H');
			Utils::printCenter("HIT", difficultySize);
		}

		setLastHit(x, y);
	}

	else if (getBoardCell(x, y) == '+') {
		explosiveMineDetonation(x, y, hit);
		Utils::printCenter("!!BOOM!!", difficultySize);
	}

	printGameBoard();
}

void PlayerBoard::updateTurnGrid(AI_Board& ai_board) {
	for(int x = 0; x < getBoardDifficultySize(); x++) {
		for(int y = 0; y < getBoardDifficultySize(); y++) {
			if( isShip(x, y) || getBoardCell(x, y) == '+') {
				ai_board.p_turnContentsGrid[x][y] = '~';
			}
			else {
				ai_board.p_turnContentsGrid[x][y] = getBoardCell(x, y);
			}
		}
	}
}
