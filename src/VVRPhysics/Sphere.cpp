#include "Sphere.h"

using namespace vvr::phys;

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

void Sphere::draw() const
{
//    glPushMatrix();

//    glTranslated(x.x, x.y, x.z);

//    glutSolidSphere(r, 13, 13);

//    glPopMatrix();
}

void Sphere::update(float t, float dt)
{
    // Collision
    handleWallCollision();

    // Forces

    // Task

    // Integration
    float* new_state = integrate(t, STATES, getState(), dt);
    setState(new_state);
    delete new_state;
}

void Sphere::handleWallCollision()
{
    Vector3 n;

    if(checkForWallCollision(x, r, n))
    {
        // Task
        v = v - n * v.dot(n) * 2;
        P = m * v;
    }
}

bool Sphere::checkForWallCollision(Vector3 &pos, float r, Vector3 &n)
{
    double BOX_SIZE = 100;

    if(pos.x-r<=-BOX_SIZE/2.0f)
    {
        //correction
        float dis = -BOX_SIZE/2.0f-(pos.x-r);
        //p.setMassCenter(pos+Vector3(dis, 0, 0));
        pos = pos+Vector3(dis, 0, 0);

        n = Wall(WALL_LEFT).getWallDirection();
    }
    else if(pos.x+r>=BOX_SIZE/2.0f)
    {
        //correction
        float dis = BOX_SIZE/2.0f-(pos.x+r);
        //p.setMassCenter(pos+Vector3(dis, 0, 0));
        pos = pos+Vector3(dis, 0, 0);

        n = Wall(WALL_RIGHT).getWallDirection();
    }
    else if(pos.y-r<=-BOX_SIZE/2.0f)
    {
        //correction
        float dis = -BOX_SIZE/2.0f-(pos.y-r);
        //p.setMassCenter(pos+Vector3(0, dis, 0));
        pos = pos+Vector3(0, dis, 0);

        n = Wall(WALL_BOTTOM).getWallDirection();
    }
    else if(pos.y+r>=BOX_SIZE/2.0f)
    {
        //correction
        float dis = BOX_SIZE/2.0f-(pos.y+r);
        //p.setMassCenter(pos+Vector3(0, dis, 0));
        pos = pos+Vector3(0, dis, 0);

        n = Wall(WALL_TOP).getWallDirection();
    }
    else if(pos.z-r<=-BOX_SIZE/2.0f)
    {
        //correction
        float dis = -BOX_SIZE/2.0f-(pos.z-r);
        //p.setMassCenter(pos+Vector3(0, 0, dis));
        pos = pos+Vector3(0, 0, dis);

        n = Wall(WALL_FAR).getWallDirection();
    }
    else if(pos.z+r>=BOX_SIZE/2.0f)
    {
        //correction
        float dis = BOX_SIZE/2.0f-(pos.z+r);
        //p.setMassCenter(pos+Vector3(0, 0, dis));
        pos = pos+Vector3(0, 0, dis);

        n = Wall(WALL_NEAR).getWallDirection();
    }
    else
    {
        return false;
    }

    return true;
}
