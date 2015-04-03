#ifndef SPHERE_H
#define SPHERE_H

#include "vvrphysicsdll.h"
#include "RigidBody.h"
#include "../VVRScene/canvas.h"

namespace vvr {
namespace phys {

class VVRPhysics_API Sphere : public RigidBody, public vvr::Sphere3D
{

public:

    float r;

    Sphere(Vector3 pos, Vector3 vel, float radius, float mass);

    void update(float t, float dt);

    void handleWallCollision();

    bool checkForWallCollision(Vector3 &pos, float r, Vector3 &v);
};

}}

#endif
