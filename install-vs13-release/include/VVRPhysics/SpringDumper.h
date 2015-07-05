#ifndef SPRING_DUMPER_H
#define SPRING_DUMPER_H

#include "vvrphysicsdll.h"
#include "Sphere.h"
#include "../VVRScene/canvas.h"

namespace vvr {
namespace phys {

class VVRPhysics_API SpringDumper : public RigidBody, public LineSeg3D // TODO: Make spring3D Shape class.
{

public:

	Sphere s1;

	float b, k, l0;

	Vector3 a;
	
	SpringDumper(Vector3 pos, Vector3 vel, float radius, float mass, 
		Vector3 anchor, float stiffness, float dumping, float restLength);
	
	void update(float t, float dt) override;
};

}}

#endif
