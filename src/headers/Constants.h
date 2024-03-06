#ifndef CONSTANTS_INCLUDED
#define CONSTANTS_INCLUDED

const int MAX_ROWS = 10;
const int MAX_COLUMNS = 10;

// orientation of the ship
enum Direction {
	horizontal, vertical
};

// (row, column) coordinate for the ship
class Coordinate {
public:
	int row;
	int column;

	Coordinate() {
		row = 0;
		column = 0;
	}

	Coordinate(int rowCoordinate, int columnCoordinate) {
		row = rowCoordinate;
		column = columnCoordinate;
	}
};

#endif // CONSTANTS_INCLUDED
