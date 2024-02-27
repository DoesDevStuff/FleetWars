/*
 * PlayerBoard.h
 *
 *  Created on: 25 Feb 2024
 *      Author: Charlie
 */

#ifndef PLAYERBOARD_H
#define PLAYERBOARD_H

#include <vector>
#include <utility>
#include "../headers/GameBoard.h"

class AI_Board;

class PlayerBoard : public GameBoard {
public:
	PlayerBoard(int chooseDifficulty, bool displayBoard) : GameBoard(chooseDifficulty, displayBoard) {
		playerChooseShips();
		playerPlaceShips();
		placeExplosiveMines();
	}

	void playerChooseShips();
	void playerPlaceShips();
	void playerFire(int&, int&);
	void fire(int, int, bool&);
	void updateTurnGrid(AI_Board&);
};



#endif /* PLAYERBOARD_H*/
