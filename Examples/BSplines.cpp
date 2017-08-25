#include <vvr/bspline.h>
#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <functional>

#define col_bg          vvr::white
#define col_line        vvr::grey
#define col_pts_ctrl    vvr::red
#define col_pts_sel     vvr::darkRed

typedef vvr::BSpline<vvr::Point3D*> spline_t;

namespace vvr {

struct Dragger2D
{
    spline_t *spline;

    Dragger2D(spline_t *spline) : spline(spline) {}

    bool grab(Drawable *dr)
    {
        if (auto pt = dynamic_cast<vvr::Point3D*>(dr)) {
            pt->colour = vvr::magenta;
        }
        return true;
    }

    void drag(Drawable *dr, int dx, int dy)
    {
        if (auto pt = dynamic_cast<vvr::Point3D*>(dr)) {
            pt->x += dx;
            pt->y += dy;
            spline->updateCurvePts(spline->getNumPts() ,true);
        }
    }

    void drop(Drawable *dr)
    {
        if (auto pt = dynamic_cast<vvr::Point3D*>(dr)) {
            pt->colour = vvr::orange;
        }
    }
};

}

struct BSplineScene : public vvr::Scene
{
    BSplineScene();
    const char* getName() const override { return "BSpline Scene"; }
    void draw() override;
    void reset() override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override { mPicker->mousePressed(x,y,modif); }
    void mouseMoved(int x, int y, int modif) override { mPicker->mouseMoved(x,y,modif);}
    void mouseReleased(int x, int y, int modif) override {mPicker->mouseReleased(x,y,modif); }

    typedef vvr::MousePicker2D<vvr::Dragger2D> picker_t;
    picker_t::Ptr mPicker;
    vvr::Canvas mCanvas;
    spline_t mSpline;
    bool mDispPts;
};

BSplineScene::BSplineScene()
{
    m_bg_col = col_bg;
    vvr::Shape::PointSize = 10;
    vvr::Shape::LineWidth = 2;
    mPicker = picker_t::Make(mCanvas, new vvr::Dragger2D(&mSpline));
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    std::vector<vvr::Point3D*> pts;
    pts.push_back(new vvr::Point3D(-150,  100 ,0));
    pts.push_back(new vvr::Point3D( -50, -100 ,0));
    pts.push_back(new vvr::Point3D(  50,  100 ,0));
    pts.push_back(new vvr::Point3D( 150, -100 ,0));
    mCanvas.clear();
    for (auto dr : pts) {
        dr->colour = vvr::orange;
        mCanvas.add(dr);
    }
    mSpline.setCtrPts(std::move(pts));
    mSpline.setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
    mSpline.updateCurvePts(13, true);
}

void BSplineScene::draw()
{
    enterPixelMode();

    auto curvpts = mSpline.getCurvePts();

    curvpts[0].draw();

    for (auto it = curvpts.begin(); it < curvpts.end() - 1; ++it)
    {
        vvr::LineSeg3D ln;
        auto p1 = *(it + 0);
        auto p2 = *(it + 1);
        ln.set(math::LineSegment(p1,p2));
        ln.draw();
        p2.draw();
    }

    mCanvas.drawif();

    exitPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    key = ::tolower(key);
    switch (key) {
    case 'p': mCanvas.toggleVisibility(); break;
    case 'r': reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP)
    {
        mSpline.updateCurvePts(mSpline.getNumPts() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN)
    {
        mSpline.updateCurvePts(mSpline.getNumPts() - 1);
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
