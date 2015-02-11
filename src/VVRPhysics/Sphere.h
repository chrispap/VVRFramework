#ifndef SPHERE_H
#define SPHERE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "Renderable.h"
#include "Wall.h"

class VVRPhysics_API Sphere : public RigidBody, public Renderable
{
public:

	float r;

	Sphere(Vector3 pos, Vector3 vel, float radius, float mass);

	~Sphere();

	virtual void draw();

	virtual void update(float t = 0);

	void handleWallCollision();

	bool checkForWallCollision(Vector3 &pos, float r, Wall &w);

};

#endif