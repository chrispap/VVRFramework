#include "scene.h"
#include "settings.h"
#include "Renderable.h"
#include <vector>

using namespace std;
using namespace vvr;
using namespace vvr::phys;

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
    Settings m_cnf;
    vector<IRenderable*> m_bodies;
    bool m_anim_on;
    float m_t, m_dt;
    float m_box_size;
    Box m_box;
};
