/*
 * BattleShip.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

#include "Ship_BaseClass.h"
#include "Constants.h"

class Battleship : public Ship_BaseClass {
public:
    Battleship() {
        setShipHealth(Constants::BATTLESHIP_SIZE);
    }
};

#endif /* BATTLESHIP_H_ */
