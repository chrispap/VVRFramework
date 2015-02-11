#include "Ex_03_PhysicsBasedSimulation.h"
#include "Viewer.h"
#include "Renderable.h"
#include "Constants.h"
#include "Box.h"
#include "Sphere.h"
#include "Cube.h"
#include "SphereContainer.h"
#include "SpringDumper.h"

using namespace vvr::phys;

PhysSimScene::PhysSimScene()
{
    // Create rigid bodies
    IRenderable* sphere = new Sphere(
        Vector3(0, 6, 0),
        Vector3(6, 6, 0),
        3, 1);
    IRenderable* cube = new Cube(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(0, 0, 1),
        5, 3);
    IRenderable* sd = new SpringDumper(
        Vector3(0, -10, 0), 
        Vector3(0, 0, 0), 2, 1,
        Vector3(0, 0, 0), 2, 1, 10);
    IRenderable* spheres = new SphereContainer();
    IRenderable* box = new Box(BOX_SIZE);

    m_renderables.push_back(cube);
    m_renderables.push_back(sphere);
    m_renderables.push_back(spheres);
    m_renderables.push_back(box);
    m_renderables.push_back(sd);
    m_t = 0;
    globPos.z = 50;
}

const char* PhysSimScene::getName() const
{
    return "Physics Based Simulation";
}

bool PhysSimScene::idle()
{
    for(unsigned i = 0;i<m_renderables.size();i++) {
        m_renderables[i]->update(m_t);
    }

    m_t += dt;

    return true;
}

void PhysSimScene::draw()
{
    for(unsigned i = 0;i<m_renderables.size();i++)
        m_renderables[i]->draw();
}
