/*
 * Carrier.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef AIRCRAFT_CARRIER_H
#define AIRCRAFT_CARRIER_H

#include "Ship_BaseClass.h"
#include "Constants.h"

class AirCraft_Carrier : public Ship_BaseClass {
public:
	AirCraft_Carrier() : Ship_BaseClass() {
		setShipHealth(Constants::AIRCRAFT_CARRIER_SIZE);
	}
};

#endif /* AIRCRAFT_CARRIER_H */
