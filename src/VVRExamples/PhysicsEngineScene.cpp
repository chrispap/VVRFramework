#include "PhysicsEngineScene.h"
#include "utils.h"
#include "Sphere.h"
#include "Cube.h"
#include "SphereContainer.h"
#include "SpringDumper.h"
#include "geom.h"

using namespace vvr;
using namespace vvr::phys;

PhysicsEngineScene::PhysicsEngineScene()
{
    // Setup scene
    m_cnf = Settings(getBasePath() + "config/settings_03.txt");
    m_box_size = m_cnf.getDbl("box_size");
    m_bg_col = Colour(m_cnf.getStr("color_bg"));

    // Create bodies
    RigidBody* sphere = new vvr::phys::Sphere(
        Vector3(0, 0, 0),
        Vector3(5, 5, 5), 3, 1);
    RigidBody* cube = new Cube(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(0, 0, 1), 5, 3);
    RigidBody* sd = new SpringDumper(
        Vector3(0, -10, 0),
        Vector3(0, 0, 0), 2, 1,
        Vector3(0, 0, 0), 2, 1, 10);
    RigidBody* spheres = new SphereContainer();

    // Keep bodies to a vector
    m_bodies.push_back(sphere);
    m_bodies.push_back(spheres);
    m_bodies.push_back(cube);
    m_bodies.push_back(sd);

    // Reset animation
    double s = m_box_size / 2;
    m_box = Box(Vec3d(-s, -s, -s), Vec3d(s, s, s));

    m_t = 0;
    m_dt = 0.09;
    m_anim_on = true;
}

PhysicsEngineScene::~PhysicsEngineScene()
{
    for (unsigned i = 0; i < m_bodies.size(); i++)
        delete m_bodies[i];
}

bool PhysicsEngineScene::idle()
{
    if (!m_anim_on)
        return false;

    for (unsigned i = 0; i < m_bodies.size(); i++) {
        m_bodies[i]->update(m_t, m_dt);
    }

    m_t += m_dt;
    return true;
}

void PhysicsEngineScene::draw()
{
    // m_box.draw(Colour::red, 20);
    m_box.draw(Colour::red, 0);
    for (unsigned i = 0; i < m_bodies.size(); i++) {
        dynamic_cast<IRenderable*>(m_bodies[i])->draw();
    }

}
void PhysicsEngineScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    if (key == ' ') {
        m_anim_on = !m_anim_on;
    }
}

void PhysicsEngineScene::arrowEvent(ArrowDir dir, int modif)
{
    if (!m_anim_on && dir == vvr::RIGHT) {
        for (unsigned i = 0; i < m_bodies.size(); i++)
            m_bodies[i]->update(m_t, m_dt);
        m_t += m_dt;
    }
}
