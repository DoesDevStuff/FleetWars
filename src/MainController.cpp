/*
 * MainController.cpp
 *
 *  Created on: 27 Feb 2024
 *      Author: Charlie
 */

#include "../src/headers/MainController.h"
#include "../src/headers/MCTS.h"

void MainController::playGame() {
		int diffChoice = 1;
	    int shotX;
	    int shotY;
	    bool turnResult = false;
	    bool playerWon = false;
	    bool aiWon = false;

	    std::cout << "Welcome to Fleet Wars!" << std::endl
	        << "Please choose your difficulty:" << std::endl
	        << "(1) Easy" << std::endl
	        << "(2) Medium" << std::endl
	        << "(3) Hard\n: ";
	    std::cin >> diffChoice;
	    while (std::cin.fail() || diffChoice < 1 || diffChoice > 3)
	    {
	        std::cin.clear();
	        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	        std::cout << "That is not a valid difficulty setting." << std::endl
	            << "Please enter a valid difficulty setting: ";
	        std::cin >> diffChoice;
	    }

	    PlayerBoard playerBoard(diffChoice, true);
	    AI_Board aiBoard(diffChoice, false);
	    MCTS mcts(&aiBoard); // Create an instance of the MCTS class

	    while (!playerBoard.checkWin(aiWon) && !aiBoard.checkWin(playerWon))
	    {
	        do
	        {
	            Utils::clearScreen();
	            std::cout << "Player's Turn\n";
	            aiBoard.printGameBoard();
	            playerBoard.playerFire(shotX, shotY);
	            aiBoard.fire(shotX, shotY, turnResult);
	            Utils::clearScreen();
	        } while (turnResult && !aiBoard.checkWin(playerWon));

	        if (playerBoard.checkWin(aiWon) || aiBoard.checkWin(playerWon))
	            break;

	        do
	        {
	            Utils::clearScreen();
	            std::cout << "AI's Turn\n";
	            playerBoard.printGameBoard();
	            Utils::pause();
	            std::pair<int, int> bestShot = mcts.selectBestShotWithMCTS(); // Call MCTS to select the best shot
	            shotX = bestShot.first;
	            shotY = bestShot.second;
	            playerBoard.fire(shotX, shotY, turnResult);
	            playerBoard.updateTurnGrid(aiBoard);
	            Utils::pause();
	        } while (turnResult && !playerBoard.checkWin(aiWon));
	    }

	    Utils::clearScreen();

	    if (playerWon)
	        std::cout << "You Win!\n\n";
	    else
	        std::cout << "You Lose...\n\n";

	    std::cout << "Thanks for playing!\nCredits: Charlie Joshi & Kyle Singer\n";
	    Utils::pause();
}
