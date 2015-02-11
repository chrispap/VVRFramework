#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "vvrphysicsdll.h"

/**
 * An abstract class for rendering and updating
 */
class VVRPhysics_API Renderable{
public:
	Renderable(){};

	~Renderable(){};

	/**
	 * Can be rendered
	 */
	virtual void draw() =0;

	/**
	 * Can be updated
	 */
	virtual void update(float t = 0) =0;

};

#endif
