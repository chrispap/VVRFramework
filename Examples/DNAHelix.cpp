#include "DNAHelix.h"
#include <vvr/utils.h>
#include <vvr/canvas.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib.h>

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace std;
using namespace vvr;
using namespace math;

HelixScene::HelixScene()
{
    m_bg_col = Colour("888888");
    m_perspective_proj = true;
    m_fullscreen = true;
    m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;
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
    const double phase = m_anim.t;

    for (float degs = -degs_from; degs < degs_from; degs += 10) 
    {
        const float t = DegToRad(phase + degs);

        p[0].x = m_r * Cos(phase + t);
        p[0].z = m_r * Sin(phase + t);
        p[0].y = m_c * t;
        Sphere3D s1(p[0].x, p[0].y, p[0].z, m_r / 8, Colour::darkRed);
        s1.setSolidRender(1);
        s1.draw();

        p[1].x = m_r * Cos(phase + t + DegToRad(90 * 2));
        p[1].z = m_r * Sin(phase + t + DegToRad(90 * 2));
        p[1].y = m_c * t;
        Sphere3D s2(p[1].x, p[1].y, p[1].z, m_r / 8, Colour::darkGreen);
        s2.setSolidRender(1);
        s2.draw();

        LineSeg3D(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, Colour(34, 34, 34)).draw();
    }

}

bool HelixScene::idle()
{
    if (m_anim.paused()) return false;
    m_anim.update();
    m_sec = vvr::getSeconds();
    return true;
}

void HelixScene::keyEvent(unsigned char key_, bool up, int modif)
{
    unsigned char key = tolower(key_);

    switch (key)
    {
    case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
    case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
    case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
    case 'n': m_style_flag ^= FLAG_SHOW_NORMALS; break;
    case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
    case ' ': m_anim.toggle(); break;
    default: Scene::keyEvent(key_, up, modif);
    }

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new HelixScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
