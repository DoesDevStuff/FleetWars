#include "../src/headers/Board.h"
#include "../src/headers/Constants.h"
#include "../src/headers/Game.h"
#include "../src/headers/Player.h"
#include "../src/headers/RandomNumberGeneration_helper.h"

#include <iostream>
#include <string>

using namespace std;


bool addStandardShips(Game& game) {

	return game.addShips(5, 'A', "aircraft carrier")  &&
		   game.addShips(4, 'B', "battleship")  &&
		   game.addShips(3, 'D', "destroyer")  &&
		   game.addShips(3, 'S', "submarine")  &&
		   game.addShips(2, 'P', "patrol boat");
}

int main () {
	const int totalGameSimulated = 10;

	cout << "Select one of these choices for an example of the game:" << endl;
	//cout << "  1.  A human player against a Hunt-Target player" << endl;
	cout << "  1.  A human player against an MCTS player" << endl;
	cout << "  2.  A standard game between a Probability recursive player and a Hunt-Target player" << endl;
	cout << "  3.  A standard game between a Hunt-Target player and an MCTS player" << endl;
	cout << "  4.  A " << totalGameSimulated << "-game match between a Hunt-Target player and an MCTS player, with no pauses" << endl;
	cout << "Enter your choice: ";

    string line;
    getline(cin,line);

    if (line.empty()) {
        cout << "You did not enter a choice" << endl;
    }

    else if (line[0] == '1') {
        Game game(10, 10);
        addStandardShips(game);
        //Player* player1 = createPlayer("huntTarget", "Hunt-Target AI", game);
        Player* player1 = createPlayer("mcts", "MCTS AI", game);
        Player* player2 = createPlayer("human", "Human Player", game);
        game.play(player1, player2);
        delete player1;
        delete player2;
    }

    else if (line[0] == '2') {
		Game game(10, 10);
		addStandardShips(game);
		Player* player1 = createPlayer("prob_recursive", "Probabilistic_Recursion AI", game);
		Player* player2 = createPlayer("huntTarget", "Hunt-Target AI", game);
		cout << "This is a standard battleship game with 5 ships on a 10x10 board" << endl << endl;
		game.play(player1, player2);
		delete player1;
		delete player2;
	}

    else if (line[0] == '3') {
		Game game(10, 10);
		addStandardShips(game);
		// createPlayer("prob_recursive", "Probabilistic_Recursion-Target AI", game);
		Player* player1 = createPlayer("huntTarget", "Hunt-Target AI", game);
		Player* player2 = createPlayer("mcts", "MCTS AI", game);
		cout << "This is a standard battleship game with 5 ships on a 10x10 board" << endl << endl;
		game.play(player1, player2);
		delete player1;
		delete player2;
	}

    else if (line[0] == '4')
    {
        int num_MCTS_AI_Wins = 0;

        for (int i = 1; i <= totalGameSimulated; i++)
        {
            cout << "============================= Game " << i << " =============================" << endl;
            Game game(10, 10);
            addStandardShips(game);
    		Player* player1 = createPlayer("huntTarget", "Hunt-Target AI", game);
    		Player* player2 = createPlayer("mcts", "MCTS AI", game);

            Player* winner = (i % 2 == 1 ? game.play(player1, player2, false) : game.play(player2, player1, false));

            if (winner == player2) {
            	num_MCTS_AI_Wins++;
            }

            delete player1;
            delete player2;
        }
        cout << "The MCTS AI player won " << num_MCTS_AI_Wins << " out of " << totalGameSimulated << " games." << endl;
    }

    else
    {
       cout << "That's not one of the choices." << endl;
    }
}
