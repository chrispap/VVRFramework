#ifndef CUBE_H
#define CUBE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "Renderable.h"

class VVRPhysics_API Cube : public RigidBody, public Renderable
{
public:

	float l;

	Cube(Vector3 pos, Vector3 vel, Vector3 omega, float length, float mass);

	~Cube();

	virtual void draw();

	virtual void update(float t = 0);

};

#endif