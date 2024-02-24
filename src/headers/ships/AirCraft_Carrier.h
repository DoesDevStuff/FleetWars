/*
 * Carrier.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef AIRCRAFT_CARRIER_H
#define AIRCRAFT_CARRIER_H

#include "Constants.h"
#include "ships/Ship_BaseClass.h"

class AirCraft_Carrier : public Ship_BaseClass {
	AirCraft_Carrier() : Ship_BaseClass() {
		setShipHealth(Constants::AIRCRAFT_CARRIER_SIZE);
	}
};

#endif /* AIRCRAFT_CARRIER_H */
