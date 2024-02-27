/*
 * ship.h
 *
 *  Created on: 22 Feb 2024
 *      Author: Charlie
 */

#ifndef SHIP_BASECLASS_H_
#define SHIP_BASECLASS_H_

class Ship_BaseClass {
private:
    int p_shipHealth;
public:
    int getShipHealth() const {
        return p_shipHealth;
    }

    void setShipHealth(int health) {
        p_shipHealth = health;
    }

    void shipIsHit() {
        setShipHealth(this->getShipHealth() - 1);
    }

    bool hasShipSunk() {
    	bool isSunk = false;

        if (this->getShipHealth() == 0) {
        	isSunk = true;
        }
        return isSunk;
    }
};

#endif /* SHIP_BASECLASS_H_ */
