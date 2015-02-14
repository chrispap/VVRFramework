#ifndef SPHERE_H
#define SPHERE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "Renderable.h"
#include "Wall.h"

namespace vvr {
namespace phys {

class VVRPhysics_API Sphere : public RigidBody, public IRenderable
{
public:

	float r;

	Sphere(Vector3 pos, Vector3 vel, float radius, float mass);

	void draw() const override;

	void update(float t, float dt);

	void handleWallCollision();

	bool checkForWallCollision(Vector3 &pos, float r, Vector3 &v);
};

}} // end namespace vvr::phys

#endif
