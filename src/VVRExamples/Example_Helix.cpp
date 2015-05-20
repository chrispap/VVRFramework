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
    m_settings = Settings(getBasePath() + CONFIG_FILE_PATH);
    m_perspective_proj = m_settings.getBool("perspective_proj");
    m_obj_col = Colour(m_settings.getStr("color_obj"));
    m_bg_col = Colour(m_settings.getStr("color_bg"));
    m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;

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
        m_r = getSceneWidth() / 30;
        m_c = getSceneWidth() / 130;
    }

    FIRST_PASS = false;
}

void HelixScene::draw()
{
    if (m_style_flag & FLAG_SHOW_AXES)
        drawAxes();

    float x1, y1, z1, x2, y2, z2;
    const float degs_from = 360 * 2;

    for (float degs = -degs_from; degs < degs_from; degs += 10) {
        const float t = math::DegToRad(degs);

        x1 = m_r * math::Cos(t);
        z1 = m_r * math::Sin(t);
        y1 = m_c * t;
        Sphere3D s1(x1, y1, z1, m_r / 10, Colour::darkRed);
        s1.setSolidRender(1);
        s1.draw();

        x2 = m_r * math::Cos(t + math::DegToRad(180));
        z2 = m_r * math::Sin(t + math::DegToRad(180));
        y2 = m_c * t;
        Sphere3D s2(x2, y2, z2, m_r / 10, Colour::darkGreen);
        s2.setSolidRender(1);
        s2.draw();

        LineSeg3D(x1, y1, z1, x2, y2, z2, Colour(34, 34, 34)).draw();
    }

}

bool HelixScene::idle()
{
    const float t = vvr::getSeconds();
    const float T = 5;

    double c = cos(2.0 * math::pi * t / T);
    m_c = getSceneWidth() / 150;
    m_c *= (c*c*0.3 + 0.7);

    Vec3d rot = m_globRot;
    rot.y = t * 20;
    m_globRot = rot;

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
