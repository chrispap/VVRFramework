#ifndef SCENE_MOLDING_H
#define SCENE_MOLDING_H

#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/drawing.h>
#include "GeoLib.h"

class MoldingScene : public vvr::Scene
{
public:
    MoldingScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    bool isFreeToMove(C2DVector &dx);

private:
    std::vector<C2DPoint> m_pts;
    vvr::Canvas m_canvas;
    C2DPoint *m_curr_p;
    C2DVector m_displacement;
    C2DVector m_dv;
    C2DPoint m_click_anchor;
    float m_last_update_time;
    bool m_anim_on;
};

#endif
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <MathGeoLib.h>

using namespace std;
using namespace vvr;

#define HARDCODED_MOLD_REMOVABLE 0
#define MOLD_SIDE_MIN_LEN 70
#define SPEED_PIXELS_PER_SEC 20

MoldingScene::MoldingScene()
{
    m_bg_col = vvr::grey;
    m_hide_log = false;
    m_fullscreen = true;
    reset();

    //! Two hardcoded molds.
    //! One that can be removed and one that cannot

    if (!HARDCODED_MOLD_REMOVABLE)
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
    m_displacement.Set(0, 0);
    m_dv.Set(0, 0);
    m_anim_on = false;
    m_curr_p = NULL;
    m_canvas.clear();
    m_pts.clear();
    m_pts.push_back(C2DPoint(-200, 0));
    m_last_update_time = vvr::getSeconds();
}

void MoldingScene::draw()
{
    enterPixelMode();

    // Draw molded object
    Colour col1 = Colour(46, 97, 162);
    C2DPolygon poly(&m_pts[0], m_pts.size(), false);
    poly.Move(m_displacement);
    vvr::draw(poly, col1, true);

    Canvas canvas;

    // Draw mold line
    float x_min_max = getViewportWidth() * 0.4;
    canvas.add(C2DPoint(-x_min_max, 0), m_pts.front(), vvr::black);
    for (int i = 0; i < (int)m_pts.size() - 1; ++i) {
        canvas.add(m_pts[i], m_pts[i + 1], vvr::black);
    }

    // Draw mold pts
    canvas.add(C2DPoint(x_min_max, 0), m_pts.back(), vvr::black);
    for (int i = 0; i < (int)m_pts.size(); ++i) {
        Colour point_col = m_curr_p != &m_pts[i] ? vvr::black : vvr::red;
        canvas.add(m_pts[i], point_col);
    }

    // Draw direction indicator circle
    if (m_anim_on) {
        C2DPoint arrow_end(m_dv.i * 30, m_dv.j * 30);
        arrow_end += m_click_anchor;
        Colour arrow_col = vvr::darkOrange;
        canvas.add(C2DCircle(m_click_anchor, 30), arrow_col);
        canvas.add(m_click_anchor, arrow_end, arrow_col);
        canvas.add(arrow_end, arrow_col);
    }

    canvas.draw();
    m_canvas.draw();

    exitPixelMode();
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
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Check if the motion is unobstructed
    //
    //!     1) Sxediaste sto m_canvas ta imikyklia 
    //!        dieythinsis kathe plevras.
    //!
    //!     2) Kante return an i kinisi einai epitrepri.
    //!
    //!     3) Vreite *an yparxei* mia epitrepti kateythinsi
    //!        kai topotheteiste tin sti metavliti `dv`.
    //!
    //!
    //! VARIABLES:
    //!
    //!  - C2DPolygon poly      // The polygon that represents the molded object 
    //!                         // in its current position.
    //!
    //!  - C2DLine side         //Anaparista mia akmi apo to kaloupi
    //!                         // Oi akmes dinontai arxizontas apo tin panw aristera
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    bool free_to_move = true;
    m_canvas.clear();

    C2DPolygon poly(&m_pts[0], m_pts.size(), false);
    C2DVector new_diplacement = m_displacement + dv;
    poly.Move(new_diplacement);

    float max_rad_from = -FLT_MAX;
    float min_rad_to = FLT_MAX;

    for (int i = 0; i < (int)m_pts.size() - 1; ++i)
    {
        C2DPoint p1 = m_pts[i];
        C2DPoint p2 = m_pts[i + 1];
        C2DLine side(p1, p2);
        float x = side.GetMidPoint().x;
        float y = side.GetMidPoint().y;
        float r = side.GetLength() * 0.45;
        vvr::Colour col;
        col = (poly.Crosses(side) || poly.Contains(side)) ? vvr::red : vvr::green;
        float dy = p2.y - p1.y;
        float dx = p2.x - p1.x;
        float rad_from = ::atan2(dy, dx);
        float rad_to = rad_from + math::pi;
        if (rad_from > max_rad_from) max_rad_from = rad_from;
        if (rad_to < min_rad_to) min_rad_to = rad_to;
        vvr::Circle2D *dir_circle_fill = new vvr::Circle2D(x, y, r, col);
        vvr::Circle2D *dir_circle_line = new vvr::Circle2D(x, y, r);
        dir_circle_fill->setRange(rad_from, rad_to);
        dir_circle_line->setRange(rad_from, rad_to);
        dir_circle_fill->filled = true;
        m_canvas.add(dir_circle_fill);
        m_canvas.add(dir_circle_line);
    }

    free_to_move = max_rad_from < min_rad_to;

    if (free_to_move)
    {
        float a = 0.5 * (max_rad_from + min_rad_to);
        dv.i = cos(a);
        dv.j = sin(a);
        m_canvas.clear();
    }

    return free_to_move;
}

void MoldingScene::mousePressed(int x, int y, int modif)
{
    C2DPoint p(x, y);

    if (altDown(modif))
    {
        for (int i = 0; i < (int)m_pts.size(); ++i) {
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
            m_click_anchor.Set(x, y);
            m_dv.Set(0, 0);
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
    C2DPoint p(x, y);

    if (altDown(modif) && m_curr_p)
    {
        *m_curr_p = p;
    }
    else if (ctrlDown(modif))
    {
        if (p.Distance(m_pts.back()) > MOLD_SIDE_MIN_LEN) {
            m_pts.push_back(p);
        }
    }
    else if (shiftDown(modif))
    {
        if (!m_anim_on) {
            m_last_update_time = vvr::getSeconds();
            m_click_anchor.Set(x, y);
        }
        C2DVector new_dir(x, y);
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

    if (key == 's')
    {
        for (int i = 0; i < (int)m_pts.size(); ++i) {
            printf("m_pts.push_back(C2DPoint(%d, %d));\n", (int)m_pts[i].x, (int)m_pts[i].y);
        }
    }
    else if (key == '0')
    {
        m_displacement.Set(0, 0);
    }

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new MoldingScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
