#ifndef SPHARE_CONTAINER_H
#define SPHARE_CONTAINER_H

#include "vvrphysicsdll.h"
#include "Renderable.h"
#include "Sphere.h"
#include "Constants.h"
#include <vector>

class VVRPhysics_API SphereContainer : public IRenderable
{

	std::vector<Sphere> spheres;

	// Task
	static const int N = 5;

public:

	SphereContainer();

	~SphereContainer();

	virtual void draw();

	virtual void update(float t = 0);

	bool checkForSpheresCollision(
		Vector3 &p, float r1, const Vector3 &q, float r2);

	float randMM(float min, float max);
};

#endif