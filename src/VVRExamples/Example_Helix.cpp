#define  NOMINMAX // Fixes a problem on windows
#include "Example_Helix.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define CONFIG_FILE_PATH "config/settings_simple3D.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace math;
using namespace vvr;

HelixScene::HelixScene()
{
    // Load settings.
    m_settings          = Settings(getBasePath() + CONFIG_FILE_PATH);
    m_bg_col            = Colour(m_settings.getStr("color_bg"));
    m_obj_col           = Colour(m_settings.getStr("color_obj"));
    m_style_flag        = FLAG_SHOW_AXES | FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;
    m_perspective_proj  = m_settings.getBool("perspective_proj");

    // Scene rotation.
    const double def_rot_x = m_settings.getDbl("def_rot_x");
    const double def_rot_y = m_settings.getDbl("def_rot_y");
    const double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRot_def = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRot_def;
}

void HelixScene::resize()
{
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_rad = getSceneWidth() / 20;
        m_c   = getSceneWidth() / 150;
    }

    FIRST_PASS = false;
}

vvr::Colour cols[] = {
    Colour::red,
    Colour::green,
    Colour::yellow,
    Colour::magenta,
};

void HelixScene::draw()
{
    if (m_style_flag & FLAG_SHOW_AXES)
        drawAxes();

    const float degs_from = 360*4;

    for (float degs = -degs_from; degs < degs_from; degs+= 2) {
        float rads = math::DegToRad(degs);
        float x = m_rad * math::Cos(rads);
        float z = m_rad * math::Sin(rads);
        float y = m_c * rads;
        Point3D(x,y,z, cols[0]).draw();
    }

    for (float degs = -degs_from; degs < degs_from; degs+= 2) {
        float rads = math::DegToRad(degs+90);
        float x = m_rad * math::Cos(rads);
        float z = m_rad * math::Sin(rads);
        float y = m_c * rads;
        Point3D(x,y,z, cols[1]).draw();
    }

}

bool HelixScene::idle()
{
    float t = vvr::getSeconds();
    const float T = 0.05;

    m_c  = getSceneWidth() / 150;
    m_c *= cos(math::DegToRad(math::pi*2/T*t));

    return true;
}

void HelixScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
    case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
    case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
    case 'n': m_style_flag ^= FLAG_SHOW_NORMALS; break;
    case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
    }

}

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new HelixScene);
}
