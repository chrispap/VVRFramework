#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <MathGeoLib.h>

class HelixScene : public vvr::Scene
{
public:
    HelixScene();
    const char* getName() const { return "Simple 3D Scene";}
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;
    bool idle() override;

private:
    vvr::Settings   m_settings;
    vvr::Colour     m_obj_col;
    float           m_r, m_c;
    int             m_style_flag;
    bool            m_anim_on;
};
