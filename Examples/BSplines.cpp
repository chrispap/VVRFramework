#include <vvr/bspline.h>
#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

#define col_bg          vvr::white
#define col_line        vvr::grey
#define col_pts_ctrl    vvr::red
#define col_pts_sel     vvr::darkRed

struct BSplineScene : public vvr::Scene
{
    BSplineScene();

    const char* getName() const override { return "BSpline Scene"; }

    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

    BSpline<C2DPoint> m_bspline;
    C2DPoint *m_curr_p;
    bool m_disp_curve_pts;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize = 25;
    vvr::Shape::LineWidth = 3;
    m_bg_col = col_bg;
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    m_curr_p = NULL;

    std::vector<C2DPoint> pts = {
        { 0, 100, },
        { 100, -100, },
        { 200, 100, },
        { 300, -100, },
    };

    for (auto &p : pts) {
        p.x -= 150;
        p *= 4;
    }

    m_bspline.setCtrPts(std::move(pts));
    m_bspline.setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
    m_bspline.updateCurve(13);
}

void BSplineScene::draw()
{
    enterPixelMode();
    auto curvpts = m_bspline.getCurvePts();
    auto p = curvpts.front();
    if (m_disp_curve_pts) vvr::Point2D(p.x, p.y, col_line).draw();
    for (auto it = curvpts.begin(); it < curvpts.end() - 1; ++it) {
        auto p1 = *(it + 0);
        auto p2 = *(it + 1);
        vvr::LineSeg2D(p1.x, p1.y, p2.x, p2.y, col_line).draw();
        if (m_disp_curve_pts) vvr::Point2D(p2.x, p2.y, col_line).draw();
    }
    for (auto &p : m_bspline.getCtrlPts()) {
        vvr::Colour col = &p == m_curr_p ? col_pts_sel : col_pts_ctrl;
        vvr::Point2D(p.x, p.y, col).draw();
    }
    returnFromPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    key = ::tolower(key);

    switch (key) {
    case 'p': m_disp_curve_pts = !m_disp_curve_pts; break;
    case 'r': reset(); break;
    }
}

void BSplineScene::mousePressed(int x, int y, int modif)
{
    C2DPoint mousept(x, y);

    for (auto &p : m_bspline.getCtrlPts()) {
        if (mousept.Distance(p) < vvr::Shape::PointSize) {
            m_curr_p = &p;
        }
    }
}

void BSplineScene::mouseMoved(int x, int y, int modif)
{
    C2DPoint mousept(x, y);

    if (m_curr_p) {
        *m_curr_p = mousept;
    }

    m_bspline.updateCurve(m_bspline.getNumPts(), true);
}

void BSplineScene::mouseReleased(int x, int y, int modif)
{
    m_curr_p = NULL;
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) {
        m_bspline.updateCurve(m_bspline.getNumPts() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN) {
        m_bspline.updateCurve(m_bspline.getNumPts() - 1);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new BSplineScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
