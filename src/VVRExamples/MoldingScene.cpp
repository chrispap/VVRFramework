#include "MoldingScene.h"
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

#define APP_TITLE "2D Molding Scene"
#define MOLD_SIDE_MIN_LEN 70
#define SPEED_PIXELS_PER_SEC 10

const char* MoldingScene::getName() const
{
    return APP_TITLE;
}

MoldingScene::MoldingScene()
{
    m_bg_col = Colour::grey;
    reset();

    // A hardcoded mold
    m_pts.push_back(C2DPoint(-128, -43));
    m_pts.push_back(C2DPoint(-76, -84));
    m_pts.push_back(C2DPoint(-90, -147));
    m_pts.push_back(C2DPoint(-59, -209));
    m_pts.push_back(C2DPoint(50, -235));
    m_pts.push_back(C2DPoint(157, -230));
    m_pts.push_back(C2DPoint(212, -193));
    m_pts.push_back(C2DPoint(271, -136));
    m_pts.push_back(C2DPoint(289, -65));
    m_pts.push_back(C2DPoint(274, 0));
}

void MoldingScene::reset()
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

void MoldingScene::draw()
{
    enterPixelMode();

    // Draw molded object
    Colour col1 = Colour(34,45,67);
    C2DPolygon poly(&m_pts[0], m_pts.size(), false);
    poly.Move(m_displacement);
    C2DPolygonSet poly_set;
    poly.GetConvexSubAreas(poly_set);
    for (int i=0; i<poly_set.size(); i++) {
        vvr::draw(*poly_set.GetAt(i), col1, true);
    }

    Canvas2D l_canvas;

    // Draw mold line
    float x_min_max = getViewportWidth() * 0.4;
    l_canvas.add(C2DPoint(-x_min_max, 0), m_pts.front(), Colour::black);
    for (int i = 0; i < (int) m_pts.size()-1; ++i) {
        l_canvas.add(m_pts[i], m_pts[i+1], Colour::black);
    }

    // Draw mold pts
    l_canvas.add(C2DPoint(x_min_max, 0), m_pts.back(), Colour::black);
    for (int i = 0; i < (int) m_pts.size(); ++i) {
        Colour point_col = m_curr_p != &m_pts[i] ? Colour::black : Colour::red;
        l_canvas.add(m_pts[i], point_col);
    }

    // Draw direction indicator circle
    if (m_anim_on) {
        C2DPoint arrow_end(m_dv.i*30, m_dv.j*30);
        arrow_end += m_click_anchor;
        Colour arrow_col = Colour::darkOrange;
        l_canvas.add(C2DCircle(m_click_anchor,30), arrow_col);
        l_canvas.add(m_click_anchor, arrow_end, arrow_col);
        l_canvas.add(arrow_end, arrow_col);
    }

    l_canvas.draw();
    m_canvas.draw();

    returnFromPixelMode();
}

bool MoldingScene::idle()
{
    if (!m_anim_on || (m_dv.i == 0 && m_dv.j == -1))
        return false;

    float t = vvr::getSeconds();
    float dt = t - m_last_update_time;
    m_displacement += m_dv * (dt * SPEED_PIXELS_PER_SEC);
    m_last_update_time = t;
    return true;
}

void MoldingScene::mousePressed(int x, int y, int modif)
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
        if (!m_anim_on) {
            m_last_update_time = vvr::getSeconds();
            m_click_anchor.Set(x,y);
            m_dv.Set(0,0);
        }
        m_anim_on = true;
    }

}

void MoldingScene::mouseReleased(int x, int y, int modif)
{
    m_curr_p = NULL;
    m_anim_on = false;
}

void MoldingScene::mouseMoved(int x, int y, int modif)
{
    C2DPoint p(x,y);

    if ( altDown(modif) && m_curr_p)
    {
        *m_curr_p = p;
    }
    else if ( ctrlDown(modif) )
    {
        if (p.Distance(m_pts.back()) > MOLD_SIDE_MIN_LEN) {
            m_pts.push_back(p);
        }
    }
    else if ( shiftDown(modif) )
    {
        if (!m_anim_on) {
            m_last_update_time = vvr::getSeconds();
            m_click_anchor.Set(x,y);
        }
        C2DVector new_dir(x,y);
        new_dir -= m_click_anchor;
        new_dir.SetLength(1);
        m_dv = new_dir;
        m_anim_on = true;
    }

}

void MoldingScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    if (key=='s')
    {
        for (int i = 0; i < (int) m_pts.size(); ++i) {
            printf("m_pts.push_back(C2DPoint(%d, %d));\n", (int) m_pts[i].x , (int) m_pts[i].y);
        }
    }
    else if (key == '0')
    {
        m_displacement.Set(0,0);
    }

}

void MoldingScene::arrowEvent(ArrowDir dir, int modif)
{

}

int main(int argc, char* argv[])
{
    return mainLoop(argc, argv, new MoldingScene);
}
