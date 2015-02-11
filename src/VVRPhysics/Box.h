#ifndef BOX_H
#define BOX_H

#include "vvrphysicsdll.h"
#include "Renderable.h"

namespace vvr {
namespace phys {

/**
 * Represents the bounding box
 */
class VVRPhysics_API Box : public Renderable {

public:

	Box(float s);

	~Box();

	void draw();

	void update(float t = 0){};

	float getSize();

private:
	float size;

};

}} // end namespace vvr::phys

#endif
