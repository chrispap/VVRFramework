#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "vvrphysicsdll.h"
#include "Matrices.h"
#include "Vectors.h"
#include "Quaternion.h"

// Task
#define USE_QUATERNIONS

#define USE_RUNGE_KUTTA

class VVRPhysics_API RigidBody
{
public:

#ifdef USE_QUATERNIONS
	static const int STATES = 13;
#else
	static const int STATES = 18;
#endif

	float m;

	Vector3 x, v, w;

	Matrix3 I_inv;

#ifdef USE_QUATERNIONS
	Quaternion q;
#else
	Matrix3 R;
#endif

	Vector3 P, L;

	Vector3 f, tau;


	RigidBody();

	~RigidBody();

	float* getState();

	void setState(float* state);

	float* dxdt(float t, int n, float u[]);

	float* integrate(float t0, int m, float u0[], float step);
};

#endif