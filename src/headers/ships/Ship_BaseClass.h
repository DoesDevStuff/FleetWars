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
        setShipHealth(getShipHealth() - 1);
    }

    bool hasShipSunk() {
        return (getShipHealth() == 0);
    }
};

#endif /* SHIP_BASECLASS_H_ */
