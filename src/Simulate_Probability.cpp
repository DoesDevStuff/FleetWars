#include "../src/headers/Board.h"
#include "../src/headers/Game.h"
#include "../src/headers/RandomNumberGeneration_helper.h"
#include <iostream>
#include <vector>
#include "headers/Simulate_Probability.h"

using namespace std;

SimulateProbability_Board::Possible_Locations::Possible_Locations(Coordinate arg_top_Left, int arg_shipID, Direction arg_direction) {
	top_Left = arg_top_Left;
	shipID = arg_shipID;
	direction = arg_direction;
}

//*********************************************************************
//  Simulate Possibilities Board Public and Helper Functions
//*********************************************************************

SimulateProbability_Board::SimulateProbability_Board(const Game& game) : p_game(game), p_locationsList(game.noOfShips(), vector<Possible_Locations>()), p_board(game.rows()*game.columns(), '.') {
    p_referenceBoard = p_board;
}

void SimulateProbability_Board::update(Coordinate coordinate, char character) {
    p_board[p_game.columns() * coordinate.row + coordinate.column] = character;
    p_referenceBoard[p_game.columns() * coordinate.row + coordinate.column] = character;
}

void SimulateProbability_Board::ship_destroyed(int shipID) {
    p_destroyedShips.push_back(shipID);
}

void SimulateProbability_Board::read_to(vector<int>& data) const {

    if (p_board.size() != data.size()) {
        return;
    }

    for (size_t i = 0, N = data.size(); i < N; ++i) {

        if (!(p_board[i] == 'o' || p_board[i] == '.') && (p_referenceBoard[i] == '.')) {
            ++data[i];
        }
    }
}

/* determine_locations saves all of the possible places that the ships can be located at
 * These are then saved in the SimulatePossibilties_Board.p_locationsList
 *
 * There is an added optimisation included in the case that we have exactly one location that exists
 * We save this location to avoid recalculating the probabilities and in this case a ship is permanently placed on our board
 * This should help provide more accuracy in calculating the other ship's locations
 */
void SimulateProbability_Board::determine_locations() {

    for (size_t i = 0, numOfShips = p_game.noOfShips(); i < numOfShips; ++i) {
    	// if there is only one option
        if (p_locationsList[i].size() == 1) {
        	//check if it valid
            if (is_valid(p_locationsList[i][0]) ) {
            	// if so skip to the next ship
                continue;
            }
        }

        // ensure that our vector is empty at the start
        p_locationsList[i].clear();

        int shipID = static_cast<int>(i);

        for (size_t i = 0, boardSize = p_board.size(); i < boardSize; ++i) {

            Coordinate coordinate(static_cast<int>(i) / p_game.columns(), static_cast<int>(i) % p_game.columns() );

            Possible_Locations possible_horizontal_location(coordinate, shipID, horizontal);

            if (is_valid(possible_horizontal_location)) {

                p_locationsList[shipID].push_back(possible_horizontal_location);
            }

            Possible_Locations possible_vertical_location(coordinate, shipID, vertical);

            if (is_valid(possible_vertical_location)) {

                p_locationsList[shipID].push_back(possible_vertical_location);
            }
        }

        // if we have exactly one option, assign it to the reference board
        if (p_locationsList[i].size() == 1) {
        	// this will save time later when we call determine_location()
            place_ship(p_locationsList[i][0]);
            p_referenceBoard = p_board;
        }
    }
}

void SimulateProbability_Board::unplace_all_ships() {
    p_board = p_referenceBoard;
}

bool SimulateProbability_Board::is_ship_destroyed(int shipID) const {
    for (size_t i = 0, destroyedShips_Size = p_destroyedShips.size(); i < destroyedShips_Size; ++i) {

        if (shipID == p_destroyedShips[i]) {
            return true;
        }
    }
    return false;
}

bool SimulateProbability_Board::is_valid_board() const {

    for (size_t i = 0, N = p_board.size(); i < N; ++i) {

        if (p_board[i] == 'X') {
            return false;
        }
    }
    return true;
}

bool SimulateProbability_Board::place_ships() {

    bool success;

    if (p_destroyedShips.size() != 0) {

        success = place_ships_recursively(p_destroyedShips[0], 1, 0);
    }

    else {

        success = place_ships_recursively(0, 0, p_destroyedShips.size() + 1);
    }

    return success;
}

bool SimulateProbability_Board::allShipsDestroyed() const {
    for (size_t i = 0, boardSize = p_board.size(); i < boardSize; ++i) {
    	//if we find a character that isn't one of those 3, its from an undamaged segment of a ship
        if (!(p_board[i] == 'o' || p_board[i] == '.' || p_board[i] == 'X')) {
        	return false;
        }
    }
    return true;
}

//*********************************************************************
//  Simulate Probability Board Private Functions
//*********************************************************************

/*
 * checks if a certain ship location (possible_Location) is valid on the board
 * To do so :
 * 1. determine if ship has sunk
 * 2. if not sunken, then ensure that the ship only crosses '.', 'X', or its own symbol
 * 3. if sunken, then ensure that the ship only crosses 'X' or its own symbol
 * 4. and also ensure that the ship did indeed cross its own symbol
 */
bool SimulateProbability_Board::is_valid(Possible_Locations possible_Location) const {

    if (possible_Location.shipID >= p_game.noOfShips()) {
        return false;
    }

    size_t start = p_game.columns() * possible_Location.top_Left.row + possible_Location.top_Left.column;

    // extra validity check to determine if the ship is has been sunk
    bool destroyed = is_ship_destroyed(possible_Location.shipID);

    bool occupies_sunk_square = false;

    // checks for those ships that aren't destroyed
    if (!destroyed) {

        if (possible_Location.direction == horizontal) {

            for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

                if ( ( ( (start + i + 1) % p_game.columns() ) == 0) && (i+1 < shipLengthByPossibleLocationID) ) {
                    // ran off the edge
                	return false;
                }

                if (!(p_board[start + i] == '.' || p_board[start + i] == 'X' || p_board[start + i] == p_game.shipSymbol(possible_Location.shipID) ) ) {
                    // overlapped with something
                	return false;
                }
            }
        }

        else {

            for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

                if (start + p_game.columns() * i > p_board.size()) {
                	// ran off the edge
                    return false;
                }

                if ( !(p_board[start + p_game.columns() * i] == '.' || p_board[start + p_game.columns() * i] == 'X' || p_board[start + p_game.columns() * i] == p_game.shipSymbol(possible_Location.shipID) ) ) {
                	// overlapped with something
                	return false;
                }
            }
        }
    }

    // check for those ships that ARE destroyed
    else {

        if (possible_Location.direction == horizontal) {

            for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

                if ( ( ( (start + i + 1) % p_game.columns() ) == 0) && (i+1 < shipLengthByPossibleLocationID) ) {
                	// run off the edge
                	return false;
                }
                // if we run over a non-hit
                if (p_board[start + i] != 'X') {

                	// if it is the ship's own symbol; that is good
                    if (p_board[start + i] == p_game.shipSymbol(possible_Location.shipID)) {

                        occupies_sunk_square = true;
                    }

                    else {
                        return false;
                    }
                }
            }
        }

        else {

            for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

                if (start + p_game.columns() * i > p_board.size() ) {
                	// run off the edge
                    return false;
                }

                // if we run over a non-hit
                if (p_board[start + p_game.columns() * i] != 'X') {

                	// if it is the ship's own symbol; that is good
                    if (p_board[start + p_game.columns() * i] == p_game.shipSymbol(possible_Location.shipID)) {

                        occupies_sunk_square = true;
                    }

                    else {
                    	return false;
                    }
                }
            }
        }
        // if we never run over a sunken square
        if (!occupies_sunk_square) {
            return false;
        }
    }

    return true;
}

bool SimulateProbability_Board::place_ship(Possible_Locations possible_Location) {

    if ( !is_valid(possible_Location) ) {
        return false;
    }

    size_t start = p_game.columns() * possible_Location.top_Left.row + possible_Location.top_Left.column;

    if (possible_Location.direction == horizontal) {

        for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {
            p_board[start + i] = p_game.shipSymbol(possible_Location.shipID);
        }

    }

    else {

        for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {
            p_board[start + p_game.columns() * i] = p_game.shipSymbol(possible_Location.shipID);
        }
    }
    return true;
}

bool SimulateProbability_Board::unplace_ship(Possible_Locations possible_Location) {

    if (possible_Location.shipID >= p_game.noOfShips()) {
        return false;
    }

    size_t start = p_game.columns() * possible_Location.top_Left.row + possible_Location.top_Left.column;

    if (possible_Location.direction == horizontal) {

        for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

            p_board[start + i] = p_referenceBoard[start + i];
        }

    }
    else {

        for(size_t i = 0, shipLengthByPossibleLocationID = p_game.shipLength(possible_Location.shipID); i < shipLengthByPossibleLocationID; ++i) {

            p_board[start + p_game.columns() * i] = p_referenceBoard[start + p_game.columns() * i];
        }
    }

    return true;
}

/* Placing ships recursively occurs in two parts :
 * 1. When we place the ships that have been sunk
 * 2. when we are simply placing the ships in order
 *
 * Every placement prompts a recursive call
 * - if the call returns false, then a different placement is tried for the current ship
 * - if there are no more possible placements, then the current function returns false
 * - if the call had returned true, then the current function returns true.
 *
 * The condition for the first return true would be :
 * if we are simply placing the ships in order and the shipID is equal to the number of ships
 *
 *
 * 	int shipID: the ship being placed
 * 	bool sunken_now : true if placing sunk ships only, false if simply placing the ships in order
 * 	size_t which_sunk : the index of destroyed_ships we are currently on
 */
bool SimulateProbability_Board::place_ships_recursively(int shipID, bool sunken_now, size_t which_sunk) {

	// return condition: not sunken_now and shipId >= our number of ships
    if (!sunken_now && shipID >= p_game.noOfShips()) {
        return true;
    }
    // shift from sunke_now to place ships in order (normal recursion) once our index is larger than destroyed ships
    if (sunken_now && which_sunk >= p_destroyedShips.size()) {
        return place_ships_recursively(0, 0, which_sunk);
    }
    // skip a step in normal recursion if the ship is in destroyed ships
    if (!sunken_now && is_ship_destroyed(shipID)) {
        return place_ships_recursively(shipID + 1, 0, which_sunk);
    }

    std::vector<int> already_guessed = {};
    size_t number_of_options = p_locationsList[shipID].size();

    bool valid_guess;
    int choice;

    /* this will ensure that we do not repeat our guesses
     * it could take time if the probabilities are limited
     * to avoid that we place the sunken ships first
     */
    while (already_guessed.size() < number_of_options) {

        do {
            valid_guess = true;
            choice = RandomNumberGeneration_helper::nextInt(static_cast<int>(number_of_options));

            for (size_t i = 0, alreadyGuessed_Size = already_guessed.size(); i < alreadyGuessed_Size; ++i) {
                if (choice == already_guessed[i]) {
                    valid_guess = false;
                }
            }
        }
        while (!valid_guess);

        already_guessed.push_back(choice);
        Possible_Locations random_location = p_locationsList[shipID][choice];

        // if the placement was successful, try to place the next ship
        // also manage what part of the recursion we are doing here
        if (place_ship(random_location)) {

            if (sunken_now) {

                if (!place_ships_recursively(p_destroyedShips[which_sunk + 1], 1, which_sunk + 1)) {
                    unplace_ship(random_location);
                }

                else {
                    return true;
                }
            }

            else {

                if (!place_ships_recursively(shipID + 1, 0, which_sunk)) {

                    unplace_ship(random_location);
                }

                else {

                    return true;
                }
            }
        }
    }

    // if we have run our of probabilities return false
    return false;
}
