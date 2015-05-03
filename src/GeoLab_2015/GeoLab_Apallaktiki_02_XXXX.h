#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

class Simple3DScene : public vvr::Scene 
{
public:
    Simple3DScene();
    const char* getName() const { return "Simple 3D Scene";}
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;

private:
    vvr::Mesh       m_icosahedron;
    vvr::Settings   m_settings;
    vvr::Colour     m_obj_col;
    float           m_sphere_rad;
    int             m_style_flag;
};
