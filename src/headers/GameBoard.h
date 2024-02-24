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
#include "ships/Ship_BaseClass.h"
#include "Utils.h"
#include "Constants.h"

using namespace std;

class GameBoard {
private:
	Utils::GAME_DIFFICULTY p_gameDifficulty;

	bool p_isShowingGameBoard;

	int p_spacesOnBoard;
	// what if we let users choose their number of ships
	int p_totalNumberOf_AircraftCarrier;
	int p_totalNumberOf_BattleShip;
	int p_totalNumberOf_Destroyer;
	int p_totalNumberOf_PatrolBoat;
	int p_totalNumberOf_Submarine;

	vector<pair<int, int>> p_lastHits; //storing coordinates of hits
	vector<Ship_BaseClass*> p_pointerToShipObj; //pointer to the ship objects
	vector<vector<char>> p_markShipStatusGrid; // this stores hits, miss, and ships in a grid
	vector<vector<char>> p_shipLocation; // stores ships coordinates. useful to get if specific ship is sunken
	vector<Utils::SHIP_TYPE> p_shipLookup; //works with p_shipLocation to determine what ship type is in p_shipLocation

public:

};



#endif /* GAMEBOARD_H */
