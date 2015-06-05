#define  NOMINMAX // Fixes a problem on windows
#include "Example_Helix.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib.h>

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
    m_anim_on = true;
}

void HelixScene::resize()
{
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_r = getSceneWidth() / 20;
        m_c = getSceneWidth() / 40;
    }

    FIRST_PASS = false;
}

void HelixScene::draw()
{
    if (m_style_flag & FLAG_SHOW_AXES)
        drawAxes();

    vec p[2];
    const float degs_from = 360 * 2;

    for (float degs = -degs_from; degs < degs_from; degs += 10) {
        const float t = DegToRad(degs);

        p[0].x = m_r * Cos(t);
        p[0].z = m_r * Sin(t);
        p[0].y = m_c * t;
        Sphere3D s1(p[0].x, p[0].y, p[0].z, m_r / 8, Colour::darkRed);
        s1.setSolidRender(1);
        s1.draw();

        p[1].x = m_r * Cos(t + DegToRad(90 * 2));
        p[1].z = m_r * Sin(t + DegToRad(90 * 2));
        p[1].y = m_c * t;
        Sphere3D s2(p[1].x, p[1].y, p[1].z, m_r / 8, Colour::darkGreen);
        s2.setSolidRender(1);
        s2.draw();

        LineSeg3D(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, Colour(34, 34, 34)).draw();
    }

}

bool HelixScene::idle()
{
    if (!m_anim_on)
        return false;

    const float t = vvr::getSeconds();
    const float V = 10;
    const float T = 20;

    if (0)
    {
        double c = cos(2.0 * pi * t / T);
        m_c = getSceneWidth() / 150;
        m_c *= (c*c*0.3 + 0.7);
    }

    if (1)
    {
        Vec3d rot = m_globRot;
        rot.y = t * V;
        m_globRot = rot;
    }

    return m_anim_on;
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
    case ' ': m_anim_on    ^= 1; break;
    }

}

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new HelixScene);
}
