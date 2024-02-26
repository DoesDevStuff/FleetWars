/*
* Constants.h
*
*  Created on: 22 Feb 2024
*      Author: Charlie
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

class Constants {
public:
	static const int NUM_OF_SHIP_TYPES = 5;

	// The ship types
	static const int AIRCRAFT_CARRIER_SIZE = 5;
	static const int BATTLESHIP_SIZE = 4;
	static const int SUBMARINE_SIZE = 3;
	static const int DESTROYER_SIZE = 3;
	static const int PATROLBOAT_SIZE = 2;

	//board sizes
	static const int EASY_SIZE = 8;
	static const int MEDIUM_SIZE = 12;
	static const int HARD_SIZE = 16;

	// spaces of board that ships can occupy
	static const int EASY_TOTAL_OCCUPIED_SPACES = 17;
	static const int MEDIUM_TOTAL_OCCUPIED_SPACES = 23;
	static const int HARD_TOTAL_OCCUPIED_SPACES = 30;
};

#endif /* CONSTANTS_H_ */
