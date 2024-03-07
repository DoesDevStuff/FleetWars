#ifndef SIMULATE_PROBABILITY
#define SIMULATE_PROBABILITY

#include "Constants.h"

using namespace std;

/* Helper function to assist our MCTS_player and the Hunt-Target / "Battleship Algorithm" player decide a the recommend_attack
 *
 * This will be designed similar to the Board but we can directly manipulate the values here
 * The main goal of the class is to be able to simulate a board placement of ships given current conditions,
 * read that board placement to an outside vector, and clear the board to allow the process to occur again
 * As efficiently as possible.
 */

class Coordinate;
class Player;

class SimulateProbability_Board
{
	struct Possible_Locations;

public:
    SimulateProbability_Board(const Game& game);

    void update(Coordinate coordinate, char character);
    void ship_destroyed(int shipID);
    void read_to(std::vector<int>& data) const;
    void determine_locations();
    void unplace_all_ships();

    bool is_ship_destroyed(int shipID) const;
    bool is_valid_board() const;
    bool place_ships();

private:
    const Game& p_game;

	bool place_ships_recursively(int shipID, bool sunken, size_t which_sunk );
    bool place_ship(Possible_Locations possible_Location);
    bool unplace_ship(Possible_Locations possible_Location);
    bool is_valid(Possible_Locations possible_Location) const;

    vector<char> p_board;
    vector<char> p_referenceBoard;
    vector<int> p_destroyedShips;
    vector<std::vector<Possible_Locations>> p_locationsList;

};

struct SimulateProbability_Board::Possible_Locations
{
    Possible_Locations(Coordinate arg_top_Left, int arg_shipID, Direction arg_direction);
    Coordinate top_Left;
    int shipID;
    Direction direction;
};

#endif // SIMULATE_PROBABILITY
