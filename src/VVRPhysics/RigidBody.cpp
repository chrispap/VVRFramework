#include "RigidBody.h"

using namespace vvr::phys;

RigidBody::RigidBody()
{
	m = 1;	
	x = v = w = P = L = f = tau = Vector3(0, 0, 0);
	q = Quaternion(Vector3(1, 0, 0), 0);
	I_inv = Matrix3(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

RigidBody::~RigidBody()
{

}

float* RigidBody::getState()
{
	float* state = new float[STATES];

	*state++ = x.x;
	*state++ = x.y;
	*state++ = x.z;
	*state++ = q.x();
	*state++ = q.y();
	*state++ = q.z();
	*state++ = q.w();
	*state++ = P.x;
	*state++ = P.y;
	*state++ = P.z;
	*state++ = L.x;
	*state++ = L.y;
	*state++ = L.z;

	state -= STATES;

	return state;
}

void RigidBody::setState(float* state)
{
	x.x = *state++;
	x.y = *state++;
	x.z = *state++;
	q.x(*state++);
	q.y(*state++);
	q.z(*state++);
	q.w(*state++);
	P.x = *state++;
	P.y = *state++;
	P.z = *state++;
	L.x = *state++;
	L.y = *state++;
	L.z = *state++;

	//momentum
	v = P / m;

	//update inertia matrix
	q.normalize();
	Matrix3 R = q.rotationMatrix();
	I_inv = R * I_inv * R.transpose();

	//angular momentum
	w = I_inv * L;
}

float* RigidBody::dxdt(float t, int n, float u[])
{
	float* x_dot = new float[STATES];

	//x_dot = u
	*x_dot++ = v.x;
	*x_dot++ = v.y;
	*x_dot++ = v.z;

	//q_dot = 1 / 2 * w * q;
	Quaternion w_hat = Quaternion(w, 0);
	Quaternion q_dot = (w_hat * q) / 2;

	*x_dot++ = q_dot.x();
	*x_dot++ = q_dot.y();
	*x_dot++ = q_dot.z();
	*x_dot++ = q_dot.w();

	//P_dot = f
	*x_dot++ = f.x;
	*x_dot++ = f.y;
	*x_dot++ = f.z;

	//L_dot = tau
	*x_dot++ = tau.x;
	*x_dot++ = tau.y;
	*x_dot++ = tau.z;

	x_dot -= STATES;

	return x_dot;
}

float* RigidBody::integrate(float t0, int n, float u0[], float step)
{

#ifdef USE_RUNGE_KUTTA
	float *f0;
	float *f1;
	float *f2;
	float *f3;
	int i;
	float t1;
	float t2;
	float t3;
	float *u;
	float *u1;
	float *u2;
	float *u3;
	//
	//  Get four sample values of the derivative.
	//
	f0 = dxdt( t0, n, u0 );

	t1 = t0 + step / 2.0f;
	u1 = new float[n];
	for ( i = 0; i < n; i++ )
	{
		u1[i] = u0[i] + step * f0[i] / 2.0f;
	}
	f1 = dxdt( t1, n, u1 );

	t2 = t0 + step / 2.0f;
	u2 = new float[n];
	for ( i = 0; i < n; i++ )
	{
		u2[i] = u0[i] + step * f1[i] / 2.0f;
	}
	f2 = dxdt( t2, n, u2 );

	t3 = t0 + step;
	u3 = new float[n];
	for ( i = 0; i < n; i++ )
	{
		u3[i] = u0[i] + step * f2[i];
	}
	f3 = dxdt( t3, n, u3 );
	//
	//  Combine them to estimate the solution.
	//
	u = new float[n];
	for ( i = 0; i < n; i++ )
	{
		u[i] = u0[i] + step * ( f0[i] + 2.0f * f1[i] + 2.0f * f2[i] + f3[i] ) / 6.0f;
	}
	//
	//  Free memory.
	//
	delete [] f0;
	delete [] f1;
	delete [] f2;
	delete [] f3;
	delete [] u1;
	delete [] u2;
	delete [] u3;

#else  //Euler

	float *dfdt;

	dfdt = dxdt(t0, n, u0);

	float *u = new float[n];

	for(int i = 0; i < n; i++ )
	{
		u[i] = u0[i] + step * dfdt[i];
	}

#endif

  return u;
}