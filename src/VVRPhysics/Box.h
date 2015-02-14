#ifndef BOX_H
#define BOX_H

#include "vvrphysicsdll.h"
#include "Renderable.h"

namespace vvr {
namespace phys {

/**
 * Represents the bounding box
 */
class VVRPhysics_API Box : public IRenderable {

public:
	Box(float s);

	void draw() const override;

	void update(float t, float dt) override {};

	float getSize();

private:
	float size;
};

}} // end namespace vvr::phys

#endif
