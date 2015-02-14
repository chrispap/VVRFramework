#include "Cube.h"
#include <GL/glut.h>

using namespace vvr::phys;

Cube::Cube(Vector3 pos, Vector3 vel, Vector3 omega,	float length, float mass)
{
    l = length;
    m = mass;
    x = pos;
    v = vel;
    P = m * v;
    w = omega;

    q = Quaternion(w, 0);
    
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

void Cube::draw() const
{
    glPushMatrix();

    glTranslated(x.x, x.y, x.z);

    glMultMatrixf(q.rotationMatrix().toGLMatrix4());

    glutSolidCube(l);

    glPopMatrix();
}

void Cube::update(float t, float dt)
{
    // - Collision

    // - Forces

    // - Integration
    float* new_state = integrate(t, STATES, getState(), dt);
    setState(new_state);
    delete new_state;
}
