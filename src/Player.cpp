#include "../src/headers/Board.h"
#include "../src/headers/Constants.h"
#include "../src/headers/Game.h"
#include "../src/headers/Player.h"
#include "../src/headers/RandomNumberGeneration_helper.h"
#include "../src/headers/Simulate_Probability.h"

#include <iostream>
#include <string>
#include <chrono>

using namespace std;

class Timer {

public:
    Timer() {
    	start();
    }

    void start() {
    	p_time = chrono::high_resolution_clock::now();
    }

    double elapsed() const {
        chrono::duration<double, milli> diff = chrono::high_resolution_clock::now() - p_time;
        return diff.count();
    }

private:
    chrono::high_resolution_clock::time_point p_time;
};

//******************************************************************************************
// 						Human Player
//******************************************************************************************

bool getLineWithTwoIntegers(int& row, int& column) {

	bool result(cin >> row >> column);

	if (!result) {
		cin.clear();
	}

	cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
	return result;
}

class HumanPlayer: public Player {

public:
	HumanPlayer(string name, const Game& game);

	bool isHumanPlayer() const override {
		return true;
	}

	bool placeShips(Board& board) override;

	Coordinate recommendAttack() override;

	virtual void recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID);

	virtual void recordAttackByOpponent(Coordinate coordinate);
};

HumanPlayer::HumanPlayer(string name, const Game& game) : Player(name, game) {}

bool HumanPlayer::placeShips(Board &board) {
	// test for terrible placements
	bool isBadPlacement = 0;
	// we give a limit on number of tries after 5 bad placements, we assume the board is incompatible with the ships and return false
	unsigned int attempts = 0;

	do {
		isBadPlacement = 0;

		for(size_t i = 0, numOfShips = game().noOfShips(); i < numOfShips; ++i) {
			bool failureTests = 0; // storing the cin failures
			int row, column = 0;
			string orientation;
			Direction direction;

			// get the position
			do {
				failureTests = 0;
				cout << "Where would you like to place your " << game().shipName(static_cast<int>(i)) << "?" << endl;
				cout << "(Input two integers designating the upper/left row & column)" << endl;

				if( !getLineWithTwoIntegers(row, column) ) {
					cout << "Invalid input, please Try Again." << endl;
					failureTests = 1;
				};
			}

			while(failureTests); // should only run in loop if cin fails

			// getting the orientation
			do {
				failureTests = 0;
				cout << "Would you like its orientation to be Vertical or Horizontal?" << endl;
				cin >> orientation;

				if (orientation == "Vertical") {
					direction = vertical;
				}
				else if (orientation == "Horizontal") {
					direction = horizontal;
				}

				else {
					cout << "Invalid input, please Try Again./nRespond with either Vertical or Horizontal." << endl;
					cin.clear();
					cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
					failureTests = 1;
				}
			}

			while (failureTests);

			// placing the ship, attempts counter only counts this section
			if (!(board.placeShips( Coordinate(row, column), static_cast<int>(i),direction ) ) ) {
				cout << "The ship does not fit in that location. Restarting ship placement" << endl;
				board.clearBoard();
				isBadPlacement = 1;
				++ attempts;
				if (attempts < 5) {break;}  // exits the for-loop, triggering the main do-while loop to restart
				else {
					cout << "You took many attempts." << endl;
					cout << "The board may be invalid, re-evaluate the coordinate points of the ship" << endl;
					return false;
				}
			}
		}
	}

	while (isBadPlacement);
	return true;
}

Coordinate HumanPlayer::recommendAttack() {
	int row, column;
	bool isfailTest = 0;

	do {
		isfailTest = 0;
		cout << "Where would you like to attack?" << endl;
		cout << "(Input two integers designating the row & column)" << endl;
		if (!getLineWithTwoIntegers(row, column)) {
			cout << "Invalid input, please Try Again." << endl;
			isfailTest = 1;
		};
	}
	// non input errors with attacking (e.x. outside of location or previously attacked)
	// are handled in the Board::attack() and Game::play()
	while (isfailTest == 1);
	return Coordinate(row, column);
}

void HumanPlayer::recordAttackResults(Coordinate, bool, bool, bool, int) {
	// completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Coordinate) {
	// ignores what opponent does
}

//******************************************************************************************
// Probabilistic Approach with Recursion - Method 2
//******************************************************************************************

/*
 * Only relying on blocking half the squares resulted in high ship density in the upper left on 10X10 boards
 *
 * We will use the attacking from this approach to implement the Hunting and Targeting approach
 *  [Reference] The algorithm for Hunt and Target is explained in length here :
 * 1. https://towardsdatascience.com/coding-an-intelligent-battleship-agent-bf0064a4b319
 * 2. https://www.youtube.com/watch?v=LbALFZoRrw8
 */

class Probabilistic_Recursion_Player : public Player {

public:
	Probabilistic_Recursion_Player(string name, const Game& game);
	bool placeShips(Board& board) override;
	Coordinate recommendAttack() override;
	virtual void recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID);
	virtual void recordAttackByOpponent(Coordinate coordinate);

private:
	unsigned int p_state;
	Coordinate p_firstHit;
	Coordinate p_previousShot;
	unsigned int p_expectedHits;
	vector<Coordinate> p_successfulHits;
	vector<Coordinate> p_hits_of_interest;
	size_t p_next_shot_index;
	vector<Coordinate> p_nextShots;
};

Probabilistic_Recursion_Player::Probabilistic_Recursion_Player(string name, const Game& game) : Player(name, game), p_state(1), p_next_shot_index(0), p_expectedHits(0) {
	//max ship length is 10, so would be a maximum of 36 of these shots to sink it after 1 hit
	p_successfulHits = {};
    p_nextShots = {
    	Coordinate(-1, 0), Coordinate(0, 1), Coordinate(1, 0), Coordinate(0, -1),
		Coordinate(-2, 0), Coordinate(0, 2), Coordinate(2, 0), Coordinate(0, -2),
		Coordinate(-3, 0), Coordinate(0, 3), Coordinate(3, 0), Coordinate(0, -3),
		Coordinate(-4, 0), Coordinate(0, 4), Coordinate(4, 0), Coordinate(0, -4),
		Coordinate(-5, 0), Coordinate(0, 5), Coordinate(5, 0), Coordinate(0, -5),
		Coordinate(-6, 0), Coordinate(0, 6), Coordinate(6, 0), Coordinate(0, -6),
		Coordinate(-7, 0), Coordinate(0, 7), Coordinate(7, 0), Coordinate(0, -7),
		Coordinate(-8, 0), Coordinate(0, 8), Coordinate(8, 0), Coordinate(0, -8),
		Coordinate(-9, 0), Coordinate(0, 9), Coordinate(9, 0), Coordinate(0, -9),
    };
};

bool placeShipsRecursively(Player &player, Board &board, int shipID) {
    /*
     * PlaceShipsRecursively is designed as a helper functions for place ships,
     * It takes a reference to the player, the board, and the specific ship to be placed
     * If the placement is successfully, placeShipsRecursively will call itself with the next ship
     *
     * If the recursion returns true, placeShipsRecursively will return true
     * And if this recursion returns false, placeShipsRecursively will try a new placement
     *
     * If placement is unsuccessful for all possible locations, placeShipsRecursively will return false,
     *
     * If the shipID is larger than any ships, then placeShipsRecursively will return true, ending the recursion
     */

    // return condition- shipID too large. i.e. all ships have been placed
    if (shipID >= player.game().noOfShips() ) {
    	return true;
    }

    int board_size = player.game().rows()*player.game().columns();
    int random_shift = RandomNumberGeneration_helper::nextInt(board_size);

    // loop through the board. If a ship is placed, recursively call the function on the next ship
    for (size_t i = random_shift, boardSize = board_size + random_shift; i < boardSize; ++i) {

    	Coordinate location;

    	if (i < board_size) { //making sure the index is valid on our board
            location = Coordinate(static_cast<int>(i) / player.game().columns(), static_cast<int>(i) % player.game().columns() );
        }

        else {
            location = Coordinate(static_cast<int>(i - board_size) / player.game().columns(), static_cast<int>(i - board_size) % player.game().columns() );
        }

        if (board.placeShips(location, shipID, horizontal)) {
        	// if the recursion is incomplete with this placement
            if (!placeShipsRecursively(player, board, shipID + 1)) {
            	// unplace the ship, and try another placement
                board.unplaceShips(location, shipID, horizontal);
            }
            // if recursion is complete, done therefore retrun true
            else {
            	return true;
            }
        }

        if (board.placeShips(location, shipID, vertical)) {

            if (!placeShipsRecursively(player, board, ++shipID)) {

                board.unplaceShips(location, shipID, vertical);
            }

            else {
            	return true;
            }
        }
    }

    // if we go throught the whole loop without recursion then need for backtracking
    return false;

}


bool Probabilistic_Recursion_Player::placeShips(Board &board) {
    // actual work done by helper function - bool placeShipsRecursively(Player &player, Board &board, int shipID)
    // if that function succeeds at placing the ships within 50 attempts, this function returns true
    unsigned int attempts = 0;

    while (attempts < 50) {
        board.blockSpacesOnBoard();
        bool success = placeShipsRecursively(*this, board, 0);
        board.unblockSpacesOnBoard();

        if (success) {
        	return true;
        }
    }
    return false;
}

Coordinate add(Coordinate first, Coordinate second) {
	return Coordinate(first.row + second.row, first.column + second.column);
}


bool search (Coordinate coordinate, const vector<Coordinate>& vec) {
    // Searching through a vector of points
    for (size_t i = 0, vecSize = vec.size(); i < vecSize; ++i) {
        if (vec[i].row == coordinate.row && vec[i].column == coordinate.column) {
        	return true;
        }
    }
    return false;
}


Coordinate Probabilistic_Recursion_Player::recommendAttack() {
    /*
     * We have 3 states here:
     * 1. random shooting, with parity
     * 2. hunting based on a specific hit, but not in the way hunt algorithms does
     * 3. The third state is cleanup, if after a ships sinks there are still unaccounted for hits on the board
     * 	then the algorithm recommends shots around them rather than return to random shooting
     */
    Coordinate recommendCoordinate;

    if (p_state == 1) {
        do {
        	recommendCoordinate = game().randomCoordinate();
        }
        while ((recommendCoordinate.row + recommendCoordinate.column) % 2 == 0); // ensuring that my guess is odd on the board
    }
    else if (p_state == 2) {
        recommendCoordinate = add(p_firstHit, p_nextShots[p_next_shot_index]);
    }
    else { //state == 3
        Coordinate interest = p_hits_of_interest[p_hits_of_interest.size() - 1]; // using the end makes cleanup easier
        recommendCoordinate = add(interest, p_nextShots[p_next_shot_index]);
    }
    return recommendCoordinate;
}

void Probabilistic_Recursion_Player::recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID) {
    /*
     * recordAttackResult adjusts the member variables of Probabilistic_Recursion_Player after each attack
     * This is done so that the recommend attack will work properly
     * The member variables it adjusts are :
     * 1. unsigned int state            	 : which state of attacking we are in, explained and employed by recommendAttack()
     * 2. Coordinate previous_shot      	 : stores the shot before the one we are recording
     * 3. vector<Coordinate> successful_hits : stores all successful hits
     * 4. unsigned int expected_hits    	 : stores expected hits which are the length of all the sunken ships
     * 5. vector<Coordinate> hits_of_interest: hits where we have some changes; first hit, changing direction within state 2, etc.
     * 6. Coordinate first_hit               : a hit while shooting was random; state 2 bases its recommends off this location
     * 7. size_t next_shots             	 : a vector with all the possible locations for a ship cells, in relation to its first hit
     * 8. vector<Coordinate> next_shot_index : which element of next shots we are using currently
     */

	// while state is 1
    if (p_state == 1) {
    	// if shot is invalid, do nothing
        if (!isValidShot) {
        	// before returning, set previous shot
            p_previousShot = coordinate;
            return;
        }

        // if the shot misses then stay in state 1
        if (isShotHit) {
        	// but if the shot hits, store successful hit
            p_successfulHits.push_back(coordinate);

            // shot hits but doesn't destroy the ship
            if (!isShipDestroyed) {
            	// Now, go to state 2
                p_state = 2;
                // and store the hit that caused the change of p_state
                p_firstHit = coordinate;
                // this first hit is now a hit of interest to us
                p_hits_of_interest.push_back(coordinate);
            }

            // if the shot hits successfully and destroys
            else {
            	// add length of the ship
                p_expectedHits += game().shipLength(shipID);
                // if there are more hits than expected
                if (p_expectedHits < p_successfulHits.size()) {
                	// In that case go to state 3
                    p_state = 3;
                }
            }

            p_next_shot_index = 0;
        }
    }

    // while state is 2
    else if (p_state == 2 ) {
    	/* Invalid shot can occur in 3 ways:
    	 * 1. We have shot and hit, continue in same direction
    	 * 2. We have shot and missed, change and go to next direction
    	 * 3. We are over the edge boundary, change and go to next direction
    	 */
        if (!isValidShot) {
            if (search(coordinate, p_successfulHits)) {
                p_next_shot_index+=4;
            }

            else {
                p_next_shot_index = (p_next_shot_index + 1) % 4;
                // change of direction stored
                p_hits_of_interest.push_back(p_previousShot);
            }
            // before returning, set coordinate to previous shot
            p_previousShot = coordinate;
            return;
        }

        // if the shot misses, go to the next index at the lowest clockwise spot
        if (!isShotHit) {
            p_next_shot_index = (p_next_shot_index + 1) % 4;
            // store change of direction
            p_hits_of_interest.push_back(p_previousShot);
        }

        else {
        	// store the successful hit and ships length
            p_successfulHits.push_back(coordinate);
            // shot hits and destroys
            if (isShipDestroyed) {
            	// add length of the ship
                p_expectedHits += game().shipLength(shipID);
                // there are more hits than expected
                if (p_expectedHits < p_successfulHits.size()) {
                    p_state = 3;
                }

                // the sunk ship was expected size
                else {
                	// return to state 1
                    p_state = 1;
                    // and clear all hits of interest
                    p_hits_of_interest = {};

                }
                // reset shot index
                p_next_shot_index = 0;

            }

            // the shot hits and doesn't destroy
            else {
            	// shot index is increased by 4 so that it proceeds in linear fashion from last hit
                p_next_shot_index +=4;
            }
        }
    }

    // while in state 3, if shot is either invalid or missed
    else {

        if (!isValidShot || !isShotHit) {
        	// but still within the cell, increase index
            if (p_next_shot_index < 3) {
                ++p_next_shot_index;
                // before returning set coordinate to previous shot
                p_previousShot = coordinate;
                return;
            }

            // if it is NOT within 1 cell reach
            else {
            	// the hit is of no interest to us
                p_hits_of_interest.pop_back();
                // reset the shot index
                p_next_shot_index = 0;

                // we have no more shots that are of interest
                if (p_hits_of_interest.size() == 0) {
                	// in that case every hit is interesting
                    p_hits_of_interest = p_successfulHits;
                }
            }
        }

        // if the shot hits successfully
        else if (isShotHit) {
        	// store the successful hit
            p_successfulHits.push_back(coordinate);

            // hits but does not destroy
            if (!isShipDestroyed) {
            	// go to state 2
                p_state = 2;
                // store the hit that caused this state change
                p_firstHit = coordinate;
                // register our hit of interest
                p_hits_of_interest.push_back(coordinate);
            }

            // shot hits and destroys ship
            else {
            	// add the ship length
                p_expectedHits += game().shipLength(shipID);
                // more hits than expected stay in state 3
                if (p_expectedHits < p_successfulHits.size()) {
                    p_state = 3;
                }

                // accurate number of hits, then return to state one, and clear the hits of interest
                else {
                    p_state = 1;
                    p_hits_of_interest = {};
                }

                // reset the shot index
                p_next_shot_index = 0;
            }
        }
    }

    p_previousShot = coordinate;
}

void Probabilistic_Recursion_Player::recordAttackByOpponent(Coordinate) {
	// ignores what the opponent does
}


//******************************************************************************************
// Hunt - Target Approach - The "battleship algorithm"
//******************************************************************************************

/* For the HuntTarget_Player the placement is done using Probabilistic_Recursion_Player placement
 * which is already using random placement
 * [Reference] - this is heavily inspired by
 * 1. https://towardsdatascience.com/coding-an-intelligent-battleship-agent-bf0064a4b319
 * 2. http://www.datagenetics.com/blog/december32011/
 *
 * Given the current shots, we randomly select the cell most likely to contain a ship.
 * Then trials the placement which occurs within helper class - Simulate_Probability board
 */

class HuntTarget_Player : public Probabilistic_Recursion_Player {
public:
	HuntTarget_Player(string name, const Game& game);
	Coordinate recommendAttack() override;
	virtual void recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID);
	virtual void recordAttackByOpponent(Coordinate coordinate);

private:
	vector<int> p_data;
	SimulateProbability_Board p_simulateProbability;
};

HuntTarget_Player::HuntTarget_Player(string name, const Game& game) : Probabilistic_Recursion_Player(name, game), p_data(game.rows()*game.columns(), 0), p_simulateProbability(game) {};

Coordinate HuntTarget_Player::recommendAttack() {
	p_simulateProbability.determine_locations();

	size_t i = 0;
	Timer timer;
	while (i < 100000) {
		// calling timer.elapsed takes time itself, so only checking every 20 simulations
		if (i % 20 == 0) {
			if (timer.elapsed() >= 3900) {
				 // break if close to 4 second limit
				cout << "TIMER FORCED BREAK" << endl;
				break;
			}
		}
		if (timer.elapsed() >= 3900) {
			// break if close to 4 second limit
			cout << "TIMER FORCED BREAK" << endl;
			break;
		}

		// recursion within possibilities, continue if failed
		if (!p_simulateProbability.place_ships()) {
			++i;
			continue;
		}

		if (p_simulateProbability.is_valid_board()) {
			// update board
			p_simulateProbability.read_to(p_data);
		}

		++i;
		p_simulateProbability.unplace_all_ships();
	}

	int max = p_data[0];
	size_t cell = 0;
	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		if (p_data[i] > max) {
			max = p_data[i]; cell = i;
		}
	}

	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		p_data[i] = 0;
	}

	// fail safe to avoid complete crash
	if (max == 0) {
		cell = static_cast<size_t>(RandomNumberGeneration_helper::nextInt(100));
	}

	return Coordinate(static_cast<int>(cell) / game().columns(), static_cast<int>(cell) % game().columns());
}

void HuntTarget_Player::recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID) {

	if (!isValidShot) {
		return;
	}

	if (!isShotHit) {
		p_simulateProbability.update(coordinate, 'o');
	}

	else {

		if (isShipDestroyed) {
			p_simulateProbability.ship_destroyed(shipID);
			p_simulateProbability.update(coordinate, game().shipSymbol(shipID));
		}

		else {
			p_simulateProbability.update(coordinate, 'X');
		}
	}

	return;
}

void HuntTarget_Player::recordAttackByOpponent(Coordinate) {
	// ignores what the opponent does
}



//******************************************************************************************
// Monte carlo Tree Search Approach
//******************************************************************************************

/* This is an attempt to make an even better algorithm to solve Battleship and make the best NPC
 * It makes use of the strategies that the Hunt - Target is using which is quite similar in approach to the
 * MCTS algorithm. We follow the same placement as that in the Hunt - Target and Probabilistic_Recursion_Player
 */

// Defining the tree node
struct TreeNode {
	int noOfVisits;
	double reward_value;
	vector<TreeNode*> children;

	TreeNode() : noOfVisits(0), reward_value(0.0) {}
};

class MCTS_Player : public HuntTarget_Player {
public:
	MCTS_Player(string name, const Game& game);
	Coordinate recommendAttack() override;
	virtual void recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID);
	virtual void recordAttackByOpponent(Coordinate coordinate);

private:
	vector<int> p_data;
	SimulateProbability_Board p_simulateProbability;

	// MCTS functions
	double UCT(TreeNode* parentNode, TreeNode* childNode);
	TreeNode* selection(TreeNode* node);
	TreeNode* expansion(TreeNode* node);
	double simulation(TreeNode* node, int shipID);
	void backpropogation(TreeNode* node, double reward);
};

MCTS_Player::MCTS_Player(string name, const Game& game) : HuntTarget_Player(name, game), p_data(game.rows()*game.columns(), 0), p_simulateProbability(game) {};

Coordinate MCTS_Player::recommendAttack() {
	Timer timer;
	TreeNode* rootNode = new TreeNode();

	while (timer.elapsed() < 3900) {
		TreeNode* selectionNode = selection(rootNode);
		TreeNode* expansionNode = expansion(selectionNode);
		double reward = simulation(expansionNode, 0); // start simulating from shipID 0
		backpropogation(expansionNode, reward);
	}

	// integrate the advanced strat from the Hunt Target Player as well
	p_simulateProbability.determine_locations();

	size_t i = 0;
	while (i < 100000) {
		// calling timer.elapsed takes time itself, so only checking every 20 simulations
		if (i % 20 == 0) {
			if (timer.elapsed() >= 3900) {
				 // break if close to 4 second limit
				cout << "TIMER FORCED BREAK" << endl;
				break;
			}
		}
		if (timer.elapsed() >= 3900) {
			// break if close to 4 second limit
			cout << "TIMER FORCED BREAK" << endl;
			break;
		}

		// recursion within possibilities, continue if failed
		if (!p_simulateProbability.place_ships()) {
			++i;
			continue;
		}

		if (p_simulateProbability.is_valid_board()) {
			// update board
			p_simulateProbability.read_to(p_data);
		}

		++i;
		p_simulateProbability.unplace_all_ships();
	}

	int max = p_data[0];
	size_t cell = 0;
	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		if (p_data[i] > max) {
			max = p_data[i]; cell = i;
		}
	}

	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		p_data[i] = 0;
	}

	// fail safe to avoid complete crash
	if (max == 0) {
		cell = static_cast<size_t>(RandomNumberGeneration_helper::nextInt(100));
	}

	return Coordinate(static_cast<int>(cell) / game().columns(), static_cast<int>(cell) % game().columns());
}

void MCTS_Player::recordAttackResults(Coordinate coordinate, bool isValidShot, bool isShotHit, bool isShipDestroyed, int shipID) {

	if (!isValidShot) {
		return;
	}

	if (!isShotHit) {
		p_simulateProbability.update(coordinate, 'o');
	}

	else {

		if (isShipDestroyed) {
			p_simulateProbability.ship_destroyed(shipID);
			p_simulateProbability.update(coordinate, game().shipSymbol(shipID));
		}

		else {
			p_simulateProbability.update(coordinate, 'X');
		}
	}

	return;
}

void MCTS_Player::recordAttackByOpponent(Coordinate) {
	// ignores what the opponent does
}

/* The MCTS steps and calculations*/

/* Calculate UCT value:
 * https://www.chessprogramming.org/UCT
 *
 * Uct =  Xi + sqrt((C * ln( n )) / ni)
 *		where
 *		Xi is the win ratio of the child
 *		n is the number of times the parent has been visited
 *		ni is the number of times the child has been visited
 *		C is a constant to adjust the amount of exploration and incorporates the sqrt(2) from the UCB1 formula
 */
double MCTS_Player::UCT(TreeNode* parentNode, TreeNode* childNode) {

	const double EXPLORATION_CONSTANT = 1.0 / (2 * sqrt(2.0));

	if(childNode->noOfVisits == 0) {
		// Return a very large value for unvisited nodes, basically like using  infinity
		return std::numeric_limits<double>::max();
	}

	double exploitation = childNode->reward_value / childNode->noOfVisits;
	double exploration = EXPLORATION_CONSTANT * sqrt(log(parentNode->noOfVisits) / childNode->noOfVisits);

	return exploitation + exploration;
}

TreeNode* MCTS_Player::selection(TreeNode* node) {
    // traverse until we reach a leaf node
    while (!node->children.empty()) {
        // select child with highest UCT score
        // Initialise to a very small value
        double bestScore = -1.0;
        TreeNode* selectedChild = nullptr;

        for (vector<TreeNode*>::iterator iterate = node->children.begin(); iterate != node->children.end(); ++iterate) {
            TreeNode* childNode = *iterate;
            double uct_score = UCT(node, childNode);

            if (uct_score > bestScore) {
                bestScore = uct_score;
                selectedChild = childNode;
            }
        }

        node = selectedChild;
    }

    return node;
}

TreeNode* MCTS_Player::expansion(TreeNode* node) {
	// add a new child to our existing node
	TreeNode* newNode = new TreeNode();
	node->children.push_back(newNode);
	return newNode;
}

double MCTS_Player::simulation(TreeNode* node, int shipID) {
	// create a copy of our simulate probability board
	SimulateProbability_Board simulation = p_simulateProbability;

	size_t i = 0;
	Timer timer;
	while (i < 100000) {
		// calling timer.elapsed takes time itself, so only checking every 20 simulations
		if (i % 20 == 0) {
			if (timer.elapsed() >= 3900) {
				 // break if close to 4 second limit
				cout << "TIMER FORCED BREAK" << endl;
				break;
			}
		}
		if (timer.elapsed() >= 3900) {
			// break if close to 4 second limit
			cout << "TIMER FORCED BREAK" << endl;
			break;
		}

		// recursion within possibilities, continue if failed
		if (!simulation.place_ships()) {
			++i;
			continue;
		}

		if (simulation.is_valid_board()) {
			// update board
			simulation.read_to(p_data);
		}

		++i;
		simulation.unplace_all_ships();
	}

	int max = p_data[0];
	size_t cell = 0;
	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		if (p_data[i] > max) {
			max = p_data[i]; cell = i;
		}
	}

	for (size_t i = 0, N = p_data.size(); i < N; ++i) {
		p_data[i] = 0;
	}

	// fail safe to avoid complete crash
	if (max == 0) {
		cell = static_cast<size_t>(RandomNumberGeneration_helper::nextInt(100));
	}

	// Return the cell index as a double reward
	return static_cast<double>(cell);
}

void MCTS_Player::backpropogation(TreeNode* node, double rewardValue) {
	// update the reward_value and the visited node count of the given node
	// and for it's parents

	while (node != nullptr) {
		node->reward_value += rewardValue;
		node->noOfVisits++;
		// move to it's parent for backpropogation
		node = nullptr;
	}
}

//******************************************************************************************
//  createPlayer
//******************************************************************************************

Player* createPlayer(string playerType, string playerName, const Game& game) {
    static string playerTypes[] = {
        "human", "prob_recursive", "huntTarget", "mcts"
    };

    int choice;
    for (choice = 0; ( (choice != sizeof(playerTypes)/sizeof(playerTypes[0]) ) && (playerType != playerTypes[choice] ) ); choice++);

    switch (choice)
    {
      case 0:  return new HumanPlayer(playerName, game);
      case 1:  return new Probabilistic_Recursion_Player(playerName, game);
      case 2:  return new HuntTarget_Player(playerName, game);
      case 3:  return new MCTS_Player(playerName, game);
      default: return nullptr;
    }
}
