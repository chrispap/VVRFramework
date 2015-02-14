#ifndef SPRING_DUMPER_H
#define SPRING_DUMPER_H

#include "vvrphysicsdll.h"
#include "Renderable.h"
#include "Sphere.h"
#include "RigidBody.h"

namespace vvr {
namespace phys {

class VVRPhysics_API SpringDumper : public IRenderable, public RigidBody
{

	Sphere s1;

	float b, k, l0;

	Vector3 a;

public:
	
	SpringDumper(Vector3 pos, Vector3 vel, float radius, float mass, 
		Vector3 anchor, float stiffness, float dumping, float restLength);
	
	void draw() const override;

	void update(float t, float dt) override;
};

}} // end namespace vvr::phys

#endif
