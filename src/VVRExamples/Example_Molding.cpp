#include "Example_Molding.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

using namespace vvr;
using std::vector;
using std::string;
using std::cout;
using std::endl;

#define APP_TITLE "2D Molding Scene"
#define HARDCODED_MOLD_REMOVABLE 0
#define MOLD_SIDE_MIN_LEN 70
#define SPEED_PIXELS_PER_SEC 20

const char* MoldingScene::getName() const
{
    return APP_TITLE;
}

MoldingScene::MoldingScene()
{
    m_bg_col = Colour::grey;
    reset();

    // Two hardcoded molds.
    // One that can be removed and one that cannot

    if ( ! HARDCODED_MOLD_REMOVABLE )
    {
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
    else
    {
        m_pts.push_back(C2DPoint(-203, -80));
        m_pts.push_back(C2DPoint(-139, -110));
        m_pts.push_back(C2DPoint(-90, -147));
        m_pts.push_back(C2DPoint(-59, -209));
        m_pts.push_back(C2DPoint(50, -235));
        m_pts.push_back(C2DPoint(157, -230));
        m_pts.push_back(C2DPoint(212, -193));
        m_pts.push_back(C2DPoint(271, -136));
        m_pts.push_back(C2DPoint(289, -65));
        m_pts.push_back(C2DPoint(300, -2));
    }
}

void MoldingScene::reset()
{
    Scene::reset();
    m_displacement.Set(0,0);
    m_dv.Set(0,0);
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
    Colour col1 = Colour(46,97,162);
    C2DPolygon poly(&m_pts[0], m_pts.size(), false);
    poly.Move(m_displacement);
    vvr::draw(poly, col1, true);

    Canvas2D canvas;

    // Draw mold line
    float x_min_max = getViewportWidth() * 0.4;
    canvas.add(C2DPoint(-x_min_max, 0), m_pts.front(), Colour::black);
    for (int i = 0; i < (int) m_pts.size()-1; ++i) {
        canvas.add(m_pts[i], m_pts[i+1], Colour::black);
    }

    // Draw mold pts
    canvas.add(C2DPoint(x_min_max, 0), m_pts.back(), Colour::black);
    for (int i = 0; i < (int) m_pts.size(); ++i) {
        Colour point_col = m_curr_p != &m_pts[i] ? Colour::black : Colour::red;
        canvas.add(m_pts[i], point_col);
    }

    // Draw direction indicator circle
    if (m_anim_on) {
        C2DPoint arrow_end(m_dv.i*30, m_dv.j*30);
        arrow_end += m_click_anchor;
        Colour arrow_col = Colour::darkOrange;
        canvas.add(C2DCircle(m_click_anchor,30), arrow_col);
        canvas.add(m_click_anchor, arrow_end, arrow_col);
        canvas.add(arrow_end, arrow_col);
    }

    canvas.draw();
    m_canvas.draw();

    returnFromPixelMode();
}

bool MoldingScene::idle()
{
    if (!m_anim_on) return false;
    float t = vvr::getSeconds();
    float dt = t - m_last_update_time;
    if (isFreeToMove(m_dv))
        m_displacement += m_dv * (dt * SPEED_PIXELS_PER_SEC);
    m_last_update_time = t;
    return true;
}

bool MoldingScene::isFreeToMove(C2DVector &dv)
{
    bool free_to_move = true;
    m_canvas.clear();

    C2DPolygon poly(&m_pts[0], m_pts.size(), false);
    C2DVector new_diplacement = m_displacement+dv;
    poly.Move(new_diplacement);

    // Check if the motion is unobstructed

    for (int i = 0; i < (int) m_pts.size()-1; ++i) {
        C2DPoint p1 = m_pts[i];
        C2DPoint p2 = m_pts[i+1];
        C2DLine side(p1, p2);

        bool intersetcs = poly.Crosses(side) || poly.Contains(side);

        Colour col = intersetcs? Colour::red : Colour::green;
        double x = side.GetMidPoint().x;
        double y = side.GetMidPoint().y;
        double r = MOLD_SIDE_MIN_LEN/4;
        double a = (p2.y-p1.y) / (p2.x-p1.x);

        double rad_from = atanf(a);
        float rad_to = rad_from + math::DegToRad(180);
        if (p2.x < p1.x) {
            float rad_tmp = rad_from;
            rad_from = rad_to - math::DegToRad(360);
            rad_to = rad_tmp;
        }

        Circle2D * dir_cir = new Circle2D(x, y, r, col);
        dir_cir->setRange(rad_from, rad_to);
        m_canvas.add(dir_cir);

        if (intersetcs) {
            m_canvas.add(side, Colour::red);
            free_to_move = false;
        }

    }

    return free_to_move;
}

void MoldingScene::mousePressed(int x, int y, int modif)
{
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
    m_canvas.clear();
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
