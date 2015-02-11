#ifndef SPRING_DUMPER_H
#define SPRING_DUMPER_H

#include "vvrphysicsdll.h"
#include "Renderable.h"
#include "Sphere.h"
#include "RigidBody.h"

class VVRPhysics_API SpringDumper : public Renderable, public RigidBody
{

	Sphere s1;

	float b, k, l0;

	Vector3 a;

public:
	
	SpringDumper(Vector3 pos, Vector3 vel, float radius, float mass, 
		Vector3 anchor, float stiffness, float dumping, float restLength);
	
	~SpringDumper();

	virtual void draw();

	virtual void update(float t = 0);
};

#endif