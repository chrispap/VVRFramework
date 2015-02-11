#include "Sphere.h"

#include "GL/glut.h"
#include "Constants.h"

Sphere::Sphere(Vector3 pos, Vector3 vel, float radius, float mass)
	: RigidBody()
{
	r = radius;
	m = mass;

	x = pos;
	v = vel;
	P = m * v;

	if(radius == 0) return;
	
	Matrix3 I = Matrix3(
		2.0f/5*mass*radius*radius, 0, 0,
		0, 2.0f/5*mass*radius*radius, 0,
		0, 0, 2.0f/5*mass*radius*radius);

	I_inv = I.invert();
}


Sphere::~Sphere()
{

}

void Sphere::draw()
{
	glPushMatrix();

	glTranslated(x.x, x.y, x.z);

	glutSolidSphere(r, 13, 13);

	glPopMatrix();
}


void Sphere::update(float t)
{
	//collision
	handleWallCollision();

	//forces
	
	// Task

	//integration
	float* new_state = integrate(t, STATES, getState(), dt);
	setState(new_state);

}

void Sphere::handleWallCollision()
{
	Wall temp;
	if(checkForWallCollision(x, r, temp))
	{
		Vector3 n = temp.getWallDirection();

		// Task
		v = v - n * v.dot(n) * 2;
		P = m * v;
	}
}

bool Sphere::checkForWallCollision(Vector3 &pos, float r, Wall &w)
{

	if(pos.x-r<=-BOX_SIZE/2.0f)
	{
		//correction
		float dis = -BOX_SIZE/2.0f-(pos.x-r);
		//p.setMassCenter(pos+Vector3(dis, 0, 0));
		pos = pos+Vector3(dis, 0, 0);

		w = Wall(WALL_LEFT);

	}
	else if(pos.x+r>=BOX_SIZE/2.0f)
	{
		//correction
		float dis = BOX_SIZE/2.0f-(pos.x+r);
		//p.setMassCenter(pos+Vector3(dis, 0, 0));
		pos = pos+Vector3(dis, 0, 0);

		w = Wall(WALL_RIGHT);

	}
	else if(pos.y-r<=-BOX_SIZE/2.0f)
	{
		//correction
		float dis = -BOX_SIZE/2.0f-(pos.y-r);
		//p.setMassCenter(pos+Vector3(0, dis, 0));
		pos = pos+Vector3(0, dis, 0);

		w = Wall(WALL_BOTTOM);

	}
	else if(pos.y+r>=BOX_SIZE/2.0f)
	{
		//correction
		float dis = BOX_SIZE/2.0f-(pos.y+r);
		//p.setMassCenter(pos+Vector3(0, dis, 0));
		pos = pos+Vector3(0, dis, 0);

		w = Wall(WALL_TOP);

	}
	else if(pos.z-r<=-BOX_SIZE/2.0f)
	{
		//correction
		float dis = -BOX_SIZE/2.0f-(pos.z-r);
		//p.setMassCenter(pos+Vector3(0, 0, dis));
		pos = pos+Vector3(0, 0, dis);

		w = Wall(WALL_FAR);

	}
	else if(pos.z+r>=BOX_SIZE/2.0f)
	{
		//correction
		float dis = BOX_SIZE/2.0f-(pos.z+r);
		//p.setMassCenter(pos+Vector3(0, 0, dis));
		pos = pos+Vector3(0, 0, dis);

		w = Wall(WALL_NEAR);

	}
	else
	{
		return false;
	}

	return true;
}


