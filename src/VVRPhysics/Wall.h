#ifndef WALL_H
#define WALL_H

#include "vvrphysicsdll.h"
#include "Vectors.h"
#include "Constants.h"

class VVRPhysics_API Wall {

public:

	Wall(){};

	/**
	 * Represents a wall based on enumerator
	 *
	 * @param (enumerator index)
	 */
	Wall(int dir);

	~Wall(void);

	/**
	 * Get wall index
	 */
	int getWall();

	/**
	 * Get wall normal direction
	 */
	Vector3 getWallDirection();

private:
	
	int direction;
};

#endif
