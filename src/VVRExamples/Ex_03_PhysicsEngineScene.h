#include "scene.h"
#include "settings.h"
#include "canvas.h"
#include "RigidBody.h"
#include <vector>

using namespace std;
using namespace vvr;
using vvr::phys::RigidBody;

class PhysicsEngineScene : public Scene
{
public: 
    PhysicsEngineScene();
    ~PhysicsEngineScene();

private:
    const char * getName() const override { return "Game Engine Example";}
    bool idle();
    void draw() override;
    void keyEvent(unsigned char key, bool up, int modif);
    void arrowEvent(ArrowDir dir, int modif);

private:
    Settings            m_cnf;
    vector<RigidBody*>  m_bodies;
    float               m_t, m_dt;
    float               m_box_size;
    bool                m_anim_on;
    Box                 m_box;
};
