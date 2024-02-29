/*
 * GameBoard.h
 *
 *  Created on: 24 Feb 2024
 *      Author: Charlie
 */

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include <utility>
#include "Utils.h"
#include "../headers/Constants.h"
#include "../headers/ships/Ship_BaseClass.h"

using namespace std;

class GameBoard {
private:
	Utils::GAME_DIFFICULTY p_gameDifficulty;

	bool p_isShowingGameBoard;

	int p_boardDifficultySize;
	int p_totalOccupiedSpacesOnBoard;
	// what if we let users choose their number of ships
	int p_totalNumberOf_AircraftCarrier;
	int p_totalNumberOf_BattleShip;
	int p_totalNumberOf_Destroyer;
	int p_totalNumberOf_PatrolBoat;
	int p_totalNumberOf_Submarine;

	vector<pair<int, int>> p_lastHits; //storing coordinates of hits
	vector<Ship_BaseClass*> p_pointerToShipObj; //pointer to the ship objects
	vector<vector<char>> p_mainGameGrid; // this stores hits, miss, and ships in a grid
	vector<vector<char>> p_shipLocation; // stores ships coordinates. useful to get if specific ship is sunken
	vector<Utils::SHIP_TYPE> p_shipLookup; //works with p_shipLocation to determine what ship type is in p_shipLocation

public:
	const int BOARD_SIZE[3] = {Constants::EASY_SIZE, Constants::MEDIUM_SIZE, Constants::HARD_SIZE};
	const int TOTAL_OCCUPIED_SPACES[3] = {Constants::EASY_TOTAL_OCCUPIED_SPACES, Constants::MEDIUM_TOTAL_OCCUPIED_SPACES, Constants::HARD_TOTAL_OCCUPIED_SPACES};
	const int SHIP_SIZE[5] = {Constants::AIRCRAFT_CARRIER_SIZE, Constants::BATTLESHIP_SIZE, Constants::DESTROYER_SIZE, Constants::PATROLBOAT_SIZE, Constants::SUBMARINE_SIZE};

	const char SHIP_REPRESENTING_LETTERS[5] = {'C', 'B', 'D', 'P', 'S'};

	const Utils::SHIP_TYPE SHIP_TYPE[5] = {Utils::aircraft_carrier, Utils::battleship, Utils::destroyer, Utils::patrolboat, Utils::submarine};
	const Utils::GAME_DIFFICULTY GAME_DIFFICULTY[3] = {Utils::easy, Utils::medium, Utils::hard};

	template <typename T> void fillBoard(vector<vector<T>> &boardVec, T fillValue) {
		int p_boardDifficultySize = getBoardDifficultySize();
		for (int y = 0; y < p_boardDifficultySize; y++){
			for (int x = 0; x < p_boardDifficultySize; x++){
				boardVec[y][x] = fillValue;
			}
		}
	}

	GameBoard(int chooseDifficulty, bool displayBoard) {
		p_isShowingGameBoard = displayBoard;
		chooseDifficulty --;

		p_gameDifficulty = GAME_DIFFICULTY[chooseDifficulty];
		p_totalOccupiedSpacesOnBoard = TOTAL_OCCUPIED_SPACES[chooseDifficulty];
		p_boardDifficultySize = BOARD_SIZE[chooseDifficulty];

		p_mainGameGrid.resize(p_boardDifficultySize, vector<char>(p_boardDifficultySize));
		p_shipLocation.resize(p_boardDifficultySize, vector<char>(p_boardDifficultySize));

		fillBoard(p_mainGameGrid, '~');
		fillBoard(p_shipLocation, '~');
	}

	virtual ~GameBoard() {
		for (auto &shipObjRef : p_pointerToShipObj) {
			delete shipObjRef;
		}
	}

	virtual void printGameBoard() const;

	// getters and setters

	Utils::GAME_DIFFICULTY getGameDifficulty() const {
		return p_gameDifficulty;
	}

	Utils::SHIP_TYPE getShipLookupCell(int ship) const {
		return p_shipLookup[ship];
	}

	Ship_BaseClass* getShipObjCell(int ship) const {
		return p_pointerToShipObj[ship];
	}

	char getBoardCell(int x, int y) const {
		return p_mainGameGrid[y][x];
	}

	char getShipLocationCell(int x, int y) const {
		return p_shipLocation[y][x];
	}

	int getBoardDifficultySize() const {
		return p_boardDifficultySize;
	}

	int getTotalOccupiedSpacesOnBoard() const{
		return p_totalOccupiedSpacesOnBoard;
	}

	int getShipLookupSize() const {
		return p_shipLookup.size();
	}

	int getAircraftCarrierCount() const {
		return p_totalNumberOf_AircraftCarrier;
	}

	int getBattleshipCount() const {
		return p_totalNumberOf_BattleShip;
	}

	int getDestroyerCount() const {
		return p_totalNumberOf_Destroyer;
	}

	int getPatrolBoatCount() const {
		return p_totalNumberOf_PatrolBoat;
	}

	int getSubmarineCount() const {
		return p_totalNumberOf_Submarine;
	}

	int getLastHitX(int hit) const {
		return get<0>(p_lastHits[hit]); // gets first element of pair in our case the row
	}

	int getLastHitY(int hit) const {
		return get<1>(p_lastHits[hit]); // get second element of pair in our case the column
	}

	int getLastHitSize() const {
		return p_lastHits.size();
	}

	void setAircraftCarrierCount(int aircraftCarrierCount) {
		p_totalNumberOf_AircraftCarrier = aircraftCarrierCount;
	}

	void setBattleshipCount(int battleShipCount) {
		p_totalNumberOf_BattleShip = battleShipCount;
	}

	void setDestroyerCount(int destroyerCount) {
		p_totalNumberOf_Destroyer = destroyerCount;
	}

	void setPatrolBoatCount(int patrolBoatCount) {
		p_totalNumberOf_PatrolBoat = patrolBoatCount;
	}

	void setSubmarineCount(int submarineCount) {
		p_totalNumberOf_Submarine = submarineCount;
	}

	void setBoardCell(int x, int y, char c) {
		p_mainGameGrid[y][x] = c;
	}

	void setShipLocationCell(int x, int y, char c) {
		p_shipLocation[y][x] = c;
	}

	void setLastHit(int x, int y) {
		p_lastHits.push_back(make_pair(x, y));
	}

	void eraseLatHit(int noOfHits) {
		p_lastHits.erase(p_lastHits.begin() + (--noOfHits));
	}

	void clearLastHit() {
		p_lastHits.clear();
	}

	void addShips(int, int, int, int, int);
	void placeShip(Utils::SHIP_TYPE, Utils::SHIP_ORIENTATION, int, int, char&);
	void shipCheck(bool&, int, bool);
	void sinkShip(Utils::SHIP_TYPE, int);
	void placeExplosiveMines();
	void explosiveMineDetonation(int x, int y, bool&);
	void explosiveMineFiredAt(int x, int y, bool&);

	bool checkShipPlacement(Utils::SHIP_TYPE, Utils::SHIP_ORIENTATION, int, int, bool) const;
	bool isShip(int, int) const;
	bool checkWin(bool& win) const {
		if( (getAircraftCarrierCount() == 0) && (getBattleshipCount() == 0) && (getDestroyerCount() == 0) && (getPatrolBoatCount() == 0) && (getSubmarineCount() == 0) ) {
			win = true;
		}
		return win;
	}
};



#endif /* GAMEBOARD_H */
