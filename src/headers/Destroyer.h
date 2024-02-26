/*
 * Destroyer.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef DESTROYER_H
#define DESTROYER_H

#include "Ship_BaseClass.h"
#include "Constants.h"

class Destroyer : public Ship_BaseClass {
public:
	Destroyer() : Ship_BaseClass() {
		setShipHealth(Constants::DESTROYER_SIZE);
	}
};


#endif /* SRC_HEADERS_DESTROYER_H_ */
