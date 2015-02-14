#ifndef CUBE_H
#define CUBE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "Renderable.h"

namespace vvr {
namespace phys {

class VVRPhysics_API Cube : public RigidBody, public IRenderable
{

public:

	float l;

	Cube(Vector3 pos, Vector3 vel, Vector3 omega, float length, float mass);

	~Cube();

	virtual void draw() const override;

	virtual void update(float t, float dt);
};

}} // end namespace vvr::phys

#endif
