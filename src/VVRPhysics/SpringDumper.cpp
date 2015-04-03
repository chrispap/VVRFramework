#include "SpringDumper.h"

using namespace vvr::phys;

SpringDumper::SpringDumper(Vector3 pos, Vector3 vel, float radius, float mass, 
    Vector3 anchor, float stiffness, float dumping, float restLength) : 
    s1(pos, vel, radius, mass), a(anchor), k(stiffness), b(dumping), l0(restLength)
{

}

void SpringDumper::update(float t, float dt)
{
    //collision

    //forces

    // Task
    s1.f = Vector3(0, 0, 0);

    s1.f += s1.m * 9.81 * Vector3(0, -1, 0);

    Vector3 x_ij = a - s1.RigidBody::x;
    s1.f += k * (x_ij.length() - l0) * x_ij.normalize() - b * s1.v;

    //integration
    float* new_state = s1.integrate(t, STATES, s1.getState(), dt);
    s1.setState(new_state);

    // Set geometric graphic quantities
    x1 = a.x;
    y1 = a.y;
    z1 = a.z;
    x2 = s1.RigidBody::x.x;
    y2 = s1.RigidBody::x.y;
    z2 = s1.RigidBody::x.z;
}
