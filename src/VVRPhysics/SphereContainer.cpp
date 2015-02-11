#include "SphereContainer.h"

#include <ctime>
#include <stdlib.h>

#include <iostream>

using namespace std;

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

SphereContainer::~SphereContainer()
{

}

void SphereContainer::draw()
{
    for(unsigned int i = 0; i < spheres.size(); i++)
    {
        spheres[i].draw();
    }
}

void SphereContainer::update(float t)
{
    //check collision
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            if(j > i)
            {
                if(checkForSpheresCollision(
                    spheres[i].x, spheres[i].r, 
                    spheres[j].x, spheres[j].r))
                {

                    Vector3 displacement = spheres[i].x - spheres[j].x;
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

    //update
    for(unsigned int i = 0; i < spheres.size(); i++)
    {
        spheres[i].update(t);
    }
}

bool SphereContainer::checkForSpheresCollision(
    Vector3 &p, float r1, const Vector3 &q, float r2)
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