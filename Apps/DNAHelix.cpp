#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/animation.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <string>
#include <MathGeoLib.h>

/*--------------------------------------------------------------------------------------*/
class HelixScene : public vvr::Scene
{
public:
    HelixScene();
    const char* getName() const override { return "DNA Helix"; }
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;
    bool idle() override;

private:
    float m_r;
    float m_c;
    double m_sec;
    vvr::Animation m_anim;
};

/*--------------------------------------------------------------------------------------*/
HelixScene::HelixScene()
{
    m_bg_col = vvr::white;
    m_perspective_proj = true;
    getGlobalAxes().hide();
}

void HelixScene::resize()
{
    if (m_first_resize) {
        m_r = getSceneWidth() / 20;
        m_c = getSceneWidth() / 40;
    }
}

void HelixScene::draw()
{
    getGlobalAxes().drawif();

    vec p[2];
    const float degs_from = 360 * 2;
    const double phase = m_anim.t();

    for (float degs = -degs_from; degs < degs_from; degs += 10)
    {
        const float t = DegToRad(phase + degs);

        p[0].x = m_r * Cos(phase + t);
        p[0].z = m_r * Sin(phase + t);
        p[0].y = m_c * t;
        vvr::Sphere3D s1(p[0].x, p[0].y, p[0].z, m_r / 8, vvr::darkRed);
        s1.filled = true;
        s1.draw();

        p[1].x = m_r * Cos(phase + t + DegToRad(90 * 2));
        p[1].z = m_r * Sin(phase + t + DegToRad(90 * 2));
        p[1].y = m_c * t;
        vvr::Sphere3D s2(p[1].x, p[1].y, p[1].z, m_r / 8, vvr::darkGreen);
        s2.filled = true;
        s2.draw();

        vvr::LineSeg3D(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, vvr::Colour(34, 34, 34)).draw();
    }
}

bool HelixScene::idle()
{
    if (m_anim.paused()) return false;
    m_anim.update();
    m_sec = vvr::get_seconds();
    return true;
}

void HelixScene::keyEvent(unsigned char key, bool up, int modif)
{
    const unsigned char k = tolower(key);

    switch (k)
    {
    case 'a': getGlobalAxes().toggle(); break;
    case ' ': m_anim.toggle(); break;
    default: Scene::keyEvent(key, up, modif);
    }

}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(HelixScene)
#endif
