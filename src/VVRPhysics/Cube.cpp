#include "Cube.h"

#include "GL/glut.h"
#include "Constants.h"

Cube::Cube(Vector3 pos, Vector3 vel, Vector3 omega,	float length, float mass)
	: RigidBody()
{
	l = length;
	m = mass;

	x = pos;
	v = vel;
	P = m * v;

	w = omega;

#ifdef USE_QUATERNIONS
	q = Quaternion(w, 0);
#endif
	
	if(l == 0) return;

	Matrix3 I = Matrix3(
		1.0f/6*mass*l*l, 0, 0,
		0, 1.0f/6*mass*l*l, 0,
		0, 0, 1.0f/6*mass*l*l);
	
	L = I * w;

	I_inv = I.invert();
}


Cube::~Cube()
{

}

void Cube::draw()
{
	glPushMatrix();

	glTranslated(x.x, x.y, x.z);

#ifdef USE_QUATERNIONS
	glMultMatrixf(q.rotationMatrix().toGLMatrix4());
#else
	glMultMatrixf(R.toGLMatrix4());
#endif
	

	glutSolidCube(l);

	glPopMatrix();
}


void Cube::update(float t)
{
	//collision

	//forces

	//integration
	float* new_state = integrate(t, STATES, getState(), dt);
	setState(new_state);

}