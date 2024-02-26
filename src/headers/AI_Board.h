/*
 * AI_Board.h
 *
 *  Created on: 25 Feb 2024
 *      Author: Charlie
 */

#ifndef AI_BOARD_H
#define AI_BOARD_H

#include <vector>
#include "Utils.h"
#include "GameBoard.h"
#include "PlayerBoard.h"

using namespace std;

class AI_Board : public GameBoard {
private:
	vector<vector<int>> p_probabilityGrid;
	vector<vector<int>> p_equalityGrid;
	vector<vector<char>> p_turnContentsGrid;

	bool p_isHitTarget = false;
	int p_successfulHitsCount = 0;

	friend void PlayerBoard::updateTurnGrid(AI_Board&);

public:
	AI_Board(int chooseDifficulty, bool displayBoard) : GameBoard(chooseDifficulty, displayBoard) {
		chooseRandomShips();
		randomiseShipPlacement();
		placeExplosiveMines();

		p_probabilityGrid.resize(getBoardDifficultySize(), vector<int>(getBoardDifficultySize()));
		p_equalityGrid.resize(getBoardDifficultySize(), vector<int>(getBoardDifficultySize()));
		p_turnContentsGrid.resize(getBoardDifficultySize(), vector<char>(getBoardDifficultySize()));

		fillBoard(p_probabilityGrid, 0);
		fillBoard(p_equalityGrid, 0);
		fillBoard(p_turnContentsGrid, '~');

		buildEqualityGrid();
	}

	void chooseRandomShips();
	void randomiseShipPlacement();
	void buildEqualityGrid();
	void generateShipPosition(int&, int&, Utils::SHIP_ORIENTATION&);
	void randomiseOrientation(Utils::SHIP_ORIENTATION&);
	void fire(int, int, bool&);
	void AI_probabilityHeuristic_Fire(int&, int&, bool&);
	void updateTurnGrid(AI_Board&);
	void updateProbaility();
	void printProbabilityGrid() const;

	int enumerateTurnsGrid() {
		//returns count of the cells / squares of the opponent whose contents we know
		int counter = 0;
		p_successfulHitsCount = 0;

		for(int i = 0; i < getBoardDifficultySize(); i++) {
			for (int j = 0; j < getBoardDifficultySize(); j++) {

				if(p_turnContentsGrid[i][j] != '~') {
					counter ++;
				}

				if(p_turnContentsGrid[i][j] != '_') {
					p_successfulHitsCount ++;
				}

			}
		}
		return counter;
	}

};

#endif /* AI_BOARD_H */
