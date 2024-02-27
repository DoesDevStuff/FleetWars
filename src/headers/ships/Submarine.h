/*
 * Submarine.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef SUBMARINE_H
#define SUBMARINE_H

#include "../../headers/Constants.h"
#include "../../headers/ships/Ship_BaseClass.h"

class Submarine : public Ship_BaseClass {
public:
	Submarine() : Ship_BaseClass(){
		setShipHealth(Constants::SUBMARINE_SIZE);
	}
};

#endif /* SUBMARINE_H */
