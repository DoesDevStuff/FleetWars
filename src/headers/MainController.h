/*
 * MainController.h
 *
 *  Created on: 27 Feb 2024
 *      Author: Charlie
 */

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <iostream>
#include <limits>
#include "../headers/PlayerBoard.h"
#include "../headers/Constants.h"
#include "../headers/AI_Board.h"
#include "../headers/MCTS.h"
#include "../headers/Utils.h"

class MainController {
private:
    bool turnResult;
    bool playerWon;
    bool AI_Won;


public:

    void playGame();
};



#endif /* MAINCONTROLLER_H */
