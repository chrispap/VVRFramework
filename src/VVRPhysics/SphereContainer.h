#ifndef SPHARE_CONTAINER_H
#define SPHARE_CONTAINER_H

#include "vvrphysicsdll.h"
#include "Renderable.h"
#include "Sphere.h"
#include <vector>

namespace vvr {
namespace phys {

class VVRPhysics_API SphereContainer : public IRenderable
{
	std::vector<Sphere> spheres;

	static const int N = 5;

public:

	SphereContainer();

	void draw() const override;

	void update(float t, float dt) override;

	bool checkForSpheresCollision(Vector3 &p, float r1, const Vector3 &q, float r2);

	float randMM(float min, float max);
};

}} // end namespace vvr::phys

#endif
