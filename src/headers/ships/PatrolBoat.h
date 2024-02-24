/*
 * PatrolBoat.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef PATROLBOAT_H
#define PATROLBOAT_H

#include "Constants.h"
#include "ships/Ship_BaseClass.h"

class PatrolBoat : public Ship_BaseClass {
public:
	PatrolBoat() : Ship_BaseClass() {
		setShipHealth(Constants::PATROLBOAT_HEALTH);
	}
};



#endif /* PATROLBOAT_H*/
