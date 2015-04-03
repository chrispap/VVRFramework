#include "SphereContainer.h"
#include <ctime>
#include <stdlib.h>
#include <iostream>

// Box
#define BOX_SIZE 100.0

// Linear velocity
#define MIN_VX  -10.0
#define MAX_VX   10.0
#define MIN_VY  -10.0
#define MAX_VY   10.0
#define MIN_VZ  -10.0
#define MAX_VZ   10.0

// Volume
#define MIN_X   -BOX_SIZE / 2.0f
#define MAX_X    BOX_SIZE / 2.0f
#define MIN_Y   -BOX_SIZE / 2.0f
#define MAX_Y    BOX_SIZE / 2.0f
#define MIN_Z   -BOX_SIZE / 2.0f
#define MAX_Z    BOX_SIZE / 2.0f

using namespace std;
using namespace vvr::phys;

SphereContainer::SphereContainer()
{
    srand((unsigned)time(NULL));

    Sphere s1 = Sphere(
        Vector3(0, 6, 0), 
        Vector3(6, 6, 0), 
        3, 1);
    spheres.push_back(s1);

    for(int i = 0; i < N; i++)
    {
        float mass = 1;
        float radius = 5;

        Sphere p = Sphere(
            Vector3(
                randMM(MIN_X + radius, MAX_X - radius),
                randMM(MIN_Y + radius, MAX_Y - radius),
                randMM(MIN_Z + radius, MAX_Z - radius)),
            Vector3(
                randMM(MIN_VX, MAX_VX),
                randMM(MIN_VX, MAX_VX),
                randMM(MIN_VX, MAX_VX)), 
            radius, mass);
        spheres.push_back(p);
    }

}

void SphereContainer::draw() const
{
    for(unsigned int i = 0; i < spheres.size(); i++) {
        spheres[i].draw();
    }
}

void SphereContainer::update(float t, float dt)
{
    // Check collision
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(j > i)
            {
                if(checkForSpheresCollision(
                    spheres[i].RigidBody::x, spheres[i].r,
                    spheres[j].RigidBody::x, spheres[j].r))
                {

                    Vector3 displacement = spheres[i].RigidBody::x - spheres[j].RigidBody::x;
                    Vector3 n = displacement.normalize();
                    Vector3 n_neg = n * (-1);

                    spheres[i].v -=  n * spheres[i].v.dot(n) * 2;
                    spheres[i].P = spheres[i].m * spheres[i].v;

                    spheres[j].v -= n_neg * spheres[j].v.dot(n_neg) * 2;
                    spheres[j].P = spheres[j].m * spheres[j].v;
                }
            }
        }
    }

    // Update
    for(unsigned int i = 0; i < spheres.size(); i++) {
        spheres[i].update(t, dt);
    }
}

bool SphereContainer::checkForSpheresCollision(Vector3 &p, float r1, const Vector3 &q, float r2)
{
    // Task
    Vector3 displacement = p - q;

    float r = (r1 + r2);

    if(displacement.lengthSq() < r * r)
    {
        //correction
        Vector3 displacement = p - q;
        float dr = (r1 + r2 - displacement.length()) / 2;
        Vector3 n = displacement.normalize();
        p = p + n * dr;

        return true;
    }
    else
    {
        return false;
    }
}

float SphereContainer::randMM(float min, float max)
{
    return (min + rand() * (max - min) / RAND_MAX);
}
