/*
 * PatrolBoat.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef PATROLBOAT_H
#define PATROLBOAT_H

#include "Ship_BaseClass.h"
#include "Constants.h"

class PatrolBoat : public Ship_BaseClass {
public:
	PatrolBoat() : Ship_BaseClass() {
		setShipHealth(Constants::PATROLBOAT_SIZE);
	}
};



#endif /* PATROLBOAT_H*/
