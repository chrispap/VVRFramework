#ifndef WALL_H
#define WALL_H

#include "vvrphysicsdll.h"
#include "Vectors.h"

namespace vvr {
namespace phys {

enum WallNorm { WALL_LEFT, WALL_RIGHT, WALL_FAR, WALL_NEAR, WALL_TOP, WALL_BOTTOM };

class VVRPhysics_API Wall {

public:

	Wall(){};

	/**
	 * Represents a wall based on enumerator
	 *
	 * @param (enumerator index)
	 */
	Wall(WallNorm dir);

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

}} // end namespace vvr::phys

#endif
