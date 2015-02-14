#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "vvrphysicsdll.h"
#include "Matrices.h"
#include "Vectors.h"
#include "Quaternion.h"

namespace vvr {
namespace phys {

class VVRPhysics_API RigidBody
{

public:

	static const int STATES = 13;

	float m;

	Vector3 x, v, w;

	Matrix3 I_inv;

	Quaternion q;

	Vector3 P, L;

	Vector3 f, tau;

	RigidBody();

	~RigidBody();

	float* getState();

	void setState(float* state);

	float* dxdt(float t, int n, float u[]);

	float* integrate(float t0, int m, float u0[], float step);
};

}} // end namespace vvr::phys

#endif
