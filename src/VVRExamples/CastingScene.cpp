#include "CastingScene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace vvr;
using std::vector;
using std::string;
using std::cout;
using std::endl;

#define APP_TITLE "Casting 2D"
#define CAST_SIDE_MIN_LEN 70
#define SPEED_PIXELS_PER_SEC 10

const char* CastingScene::getName() const
{
    return APP_TITLE;
}

CastingScene::CastingScene()
{
    m_bg_col = Colour::grey;
    reset();

    // A hardcoded mold
    m_pts.push_back(C2DPoint(-86, -20));
    m_pts.push_back(C2DPoint(-50, -55));
    m_pts.push_back(C2DPoint(-24, -142));
    m_pts.push_back(C2DPoint(-40, -224));
    m_pts.push_back(C2DPoint(-61, -273));
    m_pts.push_back(C2DPoint(-45, -347));
    m_pts.push_back(C2DPoint(-6, -393));
    m_pts.push_back(C2DPoint(67, -414));
    m_pts.push_back(C2DPoint(148, -424));
    m_pts.push_back(C2DPoint(220, -411));
    m_pts.push_back(C2DPoint(291, -366));
    m_pts.push_back(C2DPoint(331, -307));
    m_pts.push_back(C2DPoint(366, -222));
    m_pts.push_back(C2DPoint(345, -145));
    m_pts.push_back(C2DPoint(366, 0));
}

void CastingScene::reset()
{
    Scene::reset();
    m_displacement.Set(0,0);
    m_dv.Set(0,-1);
    m_anim_on = false;
    m_curr_p = NULL;
    m_canvas.clear();
    m_pts.clear();
    m_pts.push_back(C2DPoint(-200,   0));
    m_last_update_time = vvr::getSeconds();
}

void CastingScene::draw()
{
    enterPixelMode();
    m_canvas.draw();

    Canvas2D l_canvas;
    float x_min_max = getViewportWidth() * 0.4;
    l_canvas.add(C2DPoint(-x_min_max, 0), m_pts.front(), Colour::black);
    for (int i = 0; i < (int) m_pts.size()-1; ++i) {
        l_canvas.add(m_pts[i], m_pts[i+1], Colour::black);
    }
    l_canvas.add(C2DPoint(x_min_max, 0), m_pts.back(), Colour::black);
    for (int i = 0; i < (int) m_pts.size(); ++i) {
        Colour point_col = m_curr_p != &m_pts[i] ? Colour::black : Colour::red;
        l_canvas.add(m_pts[i], point_col);
    }
    for (int i = 0; i < (int) m_pts.size(); ++i) {
        C2DPoint p1 = m_pts[i] + m_displacement;
        C2DPoint p2 = m_pts[(i+1)%m_pts.size()] + m_displacement;
        l_canvas.add(p1, p2, Colour::white);
    }
    C2DPoint arrow_end(m_dv.i*30, m_dv.j*30);
    Colour arrow_col  = (m_dv.i==0 && m_dv.j==-1)? Colour::black : Colour::cyan;
    l_canvas.add(C2DPoint(0,0), arrow_end, arrow_col);
    l_canvas.add(arrow_end, arrow_col);
    l_canvas.draw();

    returnFromPixelMode();
}

bool CastingScene::idle()
{
    if (!m_anim_on || (m_dv.i == 0 && m_dv.j == -1))
        return false;

    float t = vvr::getSeconds();
    float dt = t - m_last_update_time;
    m_displacement += m_dv * (dt * SPEED_PIXELS_PER_SEC);
    m_last_update_time = t;
    echo(m_displacement.i);
    echo(m_displacement.j);
    return true;
}

void CastingScene::mousePressed(int x, int y, int modif)
{
    echo(x); echo(y);
    C2DPoint p(x,y);

    if ( altDown(modif) )
    {
        for (int i = 0; i < (int) m_pts.size(); ++i) {
            if (p.Distance(m_pts[i]) < 10) {
                m_curr_p = &m_pts[i];
            }
        }
    }
    else if (ctrlDown(modif))
    {
        m_pts.push_back(p);
    }
    else if (shiftDown(modif))
    {
        if (!m_anim_on) m_last_update_time = vvr::getSeconds();
        m_anim_on = true;
    }

}

void CastingScene::mouseReleased(int x, int y, int modif)
{
    m_curr_p = NULL;
    m_anim_on = false;
}

void CastingScene::mouseMoved(int x, int y, int modif)
{
    C2DPoint p(x,y);

    if ( altDown(modif) && m_curr_p)
    {
        *m_curr_p = p;
    }
    else if ( ctrlDown(modif) )
    {
        if (p.Distance(m_pts.back()) > CAST_SIDE_MIN_LEN) {
            m_pts.push_back(p);
        }
    }
    else if ( shiftDown(modif) )
    {
        C2DVector new_dir(x,y);
        new_dir.SetLength(1);
        m_dv = new_dir;
        if (!m_anim_on) m_last_update_time = vvr::getSeconds();
        m_anim_on = true;
    }

}

void CastingScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    for (int i = 0; i < (int) m_pts.size(); ++i) {
        printf("m_pts.push_back(C2DPoint(%d, %d));\n", (int) m_pts[i].x , (int) m_pts[i].y);
    }

}

void CastingScene::arrowEvent(ArrowDir dir, int modif)
{

}

int main(int argc, char* argv[])
{
    return mainLoop(argc, argv, new CastingScene);
}
