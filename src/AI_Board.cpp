/*
 * AI_Board.cpp
 *
 *  Created on: 26 Feb 2024
 *      Author: Charlie
 */

#include <iostream>
#include <vector>
#include <utility>
#include <functional>

#include "../src/headers/Utils.h"
#include "../src/headers/Constants.h"
#include "../src/headers/RandomNumberGeneration_helper.h"
#include "../src/headers/GameBoard.h"
#include "../src/headers/AI_Board.h"

using namespace std;

void AI_Board::chooseRandomShips() {
	int totalOccupiedSpacesOnBoard = getTotalOccupiedSpacesOnBoard();
	int shipChoice;
	int aircraftCarrierCount = 0;
	int battleshipCount = 0;
	int destroyerCount = 0;
	int patrolboatCount = 0;
	int submarineCount = 0;
	int currentShipFleet = 0;
	int randomInt = RandomNumberGeneration_helper::nextInt(0, 4);

	bool isOccupiedSpacesEmpty = false;

	reference_wrapper<int> playerShipFleet[5] = {aircraftCarrierCount, battleshipCount, destroyerCount, patrolboatCount, submarineCount};

	while(!isOccupiedSpacesEmpty) {
		shipChoice = randomInt;

		if( (totalOccupiedSpacesOnBoard - currentShipFleet) >= 2) {

			if ( (totalOccupiedSpacesOnBoard - (currentShipFleet + SHIP_SIZE[shipChoice]) ) >= 0) {
				playerShipFleet[shipChoice]++;
				currentShipFleet += SHIP_SIZE[shipChoice];
			}

			else {
				shipCheck(isOccupiedSpacesEmpty, totalOccupiedSpacesOnBoard, true);
			}
		}

		else {
			isOccupiedSpacesEmpty = false;
		}
	}

	addShips(aircraftCarrierCount, battleshipCount, destroyerCount, patrolboatCount, submarineCount);
}

void AI_Board::randomiseShipPlacement() {
	int x;
	int y;
	bool forcePlacement = false;
	char shipCounter = 64; // ascii value because the ships are ordered by letters and this is basically 1 value before A
	Utils::SHIP_ORIENTATION shipOrient;
	int shipsToAdd[5] = { getAircraftCarrierCount(), getBattleshipCount(), getDestroyerCount(), getPatrolBoatCount(), getSubmarineCount() };

	if (getGameDifficulty() != Utils::easy) {
		forcePlacement = true;
	}

	for (int i = 0; i < Constants::NUM_OF_SHIP_TYPES; i++) {

		for (int j = 0; j < shipsToAdd[i]; j++) {
			do {
				generateShipPosition(x, y, shipOrient);
			}
			while (!checkShipPlacement(SHIP_TYPE[i], shipOrient, x, y, forcePlacement));
			placeShip(SHIP_TYPE[i], shipOrient, x, y, shipCounter);
		}
	}
}

void AI_Board::buildEqualityGrid() {
	int polarity = RandomNumberGeneration_helper::nextInt(0, 1);
	int difficultySize = getBoardDifficultySize();
	int smallestShip = SHIP_SIZE[4]; //Constants::PatrolBoat = 2; which is our smallest ship is at index three. (alphabetical order)

	for (int y = 0; y < difficultySize; y++) {
		for (int x = y % smallestShip; x < difficultySize; x += smallestShip) {
			p_equalityGrid[y][x] = 1;
		}
	}

	// randomly choose from the equalities (parities)
	if (polarity == 1) {
		for (int y = 0; y < difficultySize; y++) {
			for (int x = 0; x < difficultySize; x++) {
				if (p_equalityGrid[y][x] == 0) {
					p_equalityGrid[y][x] = 1;
				}
				else {
					p_equalityGrid[y][x] = 0;
				}
			}
		}
	}
}

void AI_Board::generateShipPosition(int& x, int& y, Utils::SHIP_ORIENTATION& orient) {
	x = RandomNumberGeneration_helper::nextInt(1, getBoardDifficultySize() - 1);
	y = RandomNumberGeneration_helper::nextInt(1, getBoardDifficultySize() - 1);

	randomiseOrientation(orient);
}

void AI_Board::randomiseOrientation(Utils::SHIP_ORIENTATION& shipOrient) {
	int orient = RandomNumberGeneration_helper::nextInt(1, 2);

	if (orient == 1) {
		shipOrient = Utils::horizontal;
	}
	else {
		shipOrient = Utils::vertical;
	}
}

void AI_Board::fire(int x, int y, bool& playerHit) {

	playerHit = false;
	int difficultySize = getBoardDifficultySize();
	Utils::clearScreen();
	cout << "Player's Turn\n";

	if(getBoardCell(x, y) == '~') {
		setBoardCell(x, y, 'X');
		Utils::printCenter("MISS", difficultySize);
	}

	else if (getBoardCell(x, y) == 'X') {
		Utils::printCenter("Missed Again ...", difficultySize);
	}

	else if (isShip(x, y)) {
		playerHit = true;
		int damagedShip = (int)(getShipLocationCell(x, y) - 65);
		getShipObjCell(damagedShip)->shipIsHit();

		if (getShipObjCell(damagedShip)->hasShipSunk() ) {

			Utils::SHIP_TYPE sunkenShip = getShipLookupCell(damagedShip);
			damagedShip = (int)(getShipLocationCell(x, y));

			setBoardCell(x, y, '_');
			sinkShip(sunkenShip, damagedShip);
			Utils::printCenter("Ship has Sunk!!", difficultySize);
		}
		else {
			setBoardCell(x, y, 'H');
			Utils::printCenter("HIT !", difficultySize);
		}

		setLastHit(x, y);
	}

	else if (getBoardCell(x, y) == 'H' || getBoardCell(x, y) == '#') {
		Utils::printCenter("Already Hit ...", difficultySize);
	}

	else if (getBoardCell(x, y) == '_') {
		Utils::printCenter("That ship has already sunk!", difficultySize);
	}

	else if(getBoardCell(x, y) == '+') {
		explosiveMineDetonation(x, y, playerHit);
		Utils::printCenter("!!BOOM!!", difficultySize);
	}

	printGameBoard();

	if(!playerHit) {
		Utils::pause();
	}
}

void AI_Board::AI_probabilityHeuristic_Fire(int& x, int& y, bool& ai_Hit) {
	static int randomFire = 5 + getBoardDifficultySize();

	int difficultySize = getBoardDifficultySize();
	int knownSquares = enumerateTurnsGrid();

	if(!p_isHitTarget) {
		p_isHitTarget = ai_Hit;
	}

	// fire randomly on the first 3 turns
	if (knownSquares < randomFire && !p_isHitTarget) {
		do {
			x = RandomNumberGeneration_helper::nextInt(0, difficultySize - 1);;
			y = RandomNumberGeneration_helper::nextInt(0, difficultySize - 1);;
		}
		while (p_equalityGrid[y][x] != 1 || p_turnContentsGrid[y][x] != '~');
	}

	else {
		updateProbaility();

		// get the most probable position to fire again
		int maxProbability = 0;
		int countOfMaximums = 0;
		vector<pair<int, int>> maximums;

		for (int y = 0; y < difficultySize; y++) {
			for (int x = 0; x < difficultySize; x++) {

				if (p_probabilityGrid[y][x] > maxProbability) {
					countOfMaximums = 0;
					maximums.clear();
					maxProbability = p_probabilityGrid[y][x];
				}

				if (p_probabilityGrid[y][x] == maxProbability) {
					maximums.push_back(make_pair(x, y));
					countOfMaximums++;
				}
			}
		}

		// assuming we have more than I square with a maximum probability
		// in that case randomly pick one of them
		if (countOfMaximums > 1) {
			int probabilityChoice = RandomNumberGeneration_helper::nextInt(0, ( (int)maximums.size() - 1) );

			x = get<0>(maximums[probabilityChoice]);
			y = get<1>(maximums[probabilityChoice]);
		}

		else {
			x = get<0>(maximums[0]);
			y = get<1>(maximums[0]);
		}
	}
}

void AI_Board::updateTurnGrid(AI_Board& ai_board) {

	for(int y = 0; y < getBoardDifficultySize(); y++) {
		for(int x = 0; x < getBoardDifficultySize(); x++) {

			if( isShip(x, y) || getBoardCell(x, y) == '+') {
				ai_board.p_turnContentsGrid[y][x] = '~';
			}

			else {
				ai_board.p_turnContentsGrid[y][x] = getBoardCell(x, y);
			}
		}
	}
}

void AI_Board::updateProbaility() {
	bool canFit;
	int hits;
	int probabilityWeights;
	int shipTypes = Constants::NUM_OF_SHIP_TYPES;
	int opp_OccupiedSpacesLeft = TOTAL_OCCUPIED_SPACES[getGameDifficulty()] - p_successfulHitsCount;
	fillBoard(p_probabilityGrid, 0);

	// we do not need to calculate probability for ships that cannot exist
	if(opp_OccupiedSpacesLeft < 5) {

		if(opp_OccupiedSpacesLeft > 2) {
			shipTypes = opp_OccupiedSpacesLeft;
		}
		else {
			shipTypes = 1;
		}
	}

	// loop of all the ship types
	for (int i = 0; i < shipTypes; i++) {

		// try placing ships horizontally
		for (int y = 0; y < getBoardDifficultySize(); y++) {

			for (int x = 0; x < (getBoardDifficultySize() - SHIP_SIZE[i] + 1 ); x++ ) {

				canFit = true;
				hits = 0;
				probabilityWeights = 1;

				for (int j = x; j < SHIP_SIZE[i] + x; j++) {

					if (p_turnContentsGrid[y][j] != '~' && p_turnContentsGrid[y][j] != 'H') {

						canFit = false;
						break;
					}

					else if (p_turnContentsGrid[y][j] == 'H') {
						hits++;
					}
				}

				if (hits > 0) {

					if (hits == 1) {
						probabilityWeights = 25;
					}

					else {

						probabilityWeights = 200 * hits;
					}
				}

				// increment probability matrix if the ship actually fits
				if (canFit) {

					for (int j = x; j < SHIP_SIZE[i] + x; j++) {
						if (hits > 0) {
							if (p_turnContentsGrid[y][j] == 'H') {
								p_probabilityGrid[y][j] = 0;
							}
							else {
								p_probabilityGrid[y][j] += probabilityWeights;
							}
						}
						else {
							p_probabilityGrid[y][j] += probabilityWeights;
						}
					}
				}
			}
		}

		// try placing the ships vertically
		for (int x = 0; x < getBoardDifficultySize(); x++) {

			for (int y = 0; y < (getBoardDifficultySize() - SHIP_SIZE[i] + 1 ); y++) {

				canFit = true;
				hits = 0;
				probabilityWeights = 1;

				for(int j = y; j < SHIP_SIZE[i] + y; j++) {
					if (p_turnContentsGrid[j][x] != '~' && p_turnContentsGrid[j][x] != 'H') {
						canFit = false;
						break;
					}
					else if (p_turnContentsGrid[j][x] == 'H') {
						hits++;
					}
				}

				if (hits > 0) {

					if (hits == 1) {
						probabilityWeights = 25;
					}
					else {
						probabilityWeights = 200 * hits;
					}
				}

				// increment the probability matrix if the ship actually fits

				if (canFit) {

					for(int j = y; j < SHIP_SIZE[i] + y; j++) {

						if (hits > 0) {
							if (p_turnContentsGrid[j][x] == 'H') {
								p_probabilityGrid[j][x] = 0;
							}
							else {
								p_probabilityGrid[j][x] += probabilityWeights;
							}
						}
						else {
							p_probabilityGrid[j][x] += probabilityWeights;
						}
					}
				}
			}
		}
	}
}

void AI_Board::printProbabilityGrid() const {

	int difficultySize = getBoardDifficultySize();

	cout << "\n\n";

	for (int y = difficultySize - 1; y >= 0; y--) {
		cout << "   ";
		for (int x = 0; x < difficultySize; x++) {
			if (p_probabilityGrid[y][x] < 0) {
				cout << p_probabilityGrid[y][x] << "     ";
			}
			else if (p_probabilityGrid[y][x] < 100) {
				cout << p_probabilityGrid[y][x] << "    ";
			}
			else if (p_probabilityGrid[y][x] < 1000) {
				cout << p_probabilityGrid[y][x] << "   ";
			}
			else {
				cout << p_probabilityGrid[y][x] << "  ";
			}
		}
		cout << endl;
	}
}
