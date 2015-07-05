#ifndef CUBE_H
#define CUBE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "../VVRScene/canvas.h"

namespace vvr {
namespace phys {

class VVRPhysics_API Cube : public RigidBody, public Box3D
{

public:

	float l;

	Cube(Vector3 pos, Vector3 vel, Vector3 omega, float length, float mass);

	void update(float t, float dt) override;
};

}}

#endif
