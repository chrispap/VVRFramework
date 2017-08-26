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

#define col_cps         vvr::darkOrange

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

    typedef vvr::BSpline<vvr::Point3D*> spline_t;
    typedef vvr::MousePicker2D<vvr::Point3D> mouse_picker_t;

    mouse_picker_t::Ptr mPicker;
    vvr::Canvas mCpsCanvas;
    spline_t mSpline;
    bool mDispCurvePts;
};

BSplineScene::BSplineScene()
{
    mPicker = mouse_picker_t::Make(&mCpsCanvas);
    reset();
}

struct RefLine : public vvr::Drawable
{
    vvr::Point3D *p1, *p2;
    RefLine()
    {
        p1 = new vvr::Point3D;
        p2 = new vvr::Point3D;
    }
    void draw() const override
    {
        vvr::LineSeg3D(math::LineSegment(*p1, *p2)).drawif();
    }
};

void BSplineScene::reset()
{
    vvr::Scene::reset();
    mCpsCanvas.clear();
    std::vector<vvr::Point3D*> cps;
    cps.push_back(new vvr::Point3D(-150,  100, 0, col_cps));
    cps.push_back(new vvr::Point3D( -50, -100, 0, col_cps));
    cps.push_back(new vvr::Point3D(  50,  100, 0, col_cps));
    cps.push_back(new vvr::Point3D( 150, -100, 0, col_cps));
    for (auto dr : cps) mCpsCanvas.add(dr);
    mSpline.setCtrPts(std::move(cps));
    mSpline.setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
    mSpline.setNumPts(16);

    auto ln = new RefLine;
    ln->p1->set({0,100,0});
    ln->p2->set({0,200,0});
    mCpsCanvas.add(ln->p1);
    mCpsCanvas.add(ln->p2);
    mCpsCanvas.add(ln);
}

void BSplineScene::draw()
{
    enterPixelMode();

    /* Draw spline curve */
    mSpline.update(true);
    auto pts = mSpline.getPts();
    for (auto it = pts.begin(); it < pts.end() - 1; ++it) {
        vvr::LineSeg3D(math::LineSegment(it[0],it[1])).draw();
        if (mDispCurvePts) it->draw();
    }
    pts.back().draw();

    /* Draw Control points */
    mCpsCanvas.drawif();

    exitPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    key = ::tolower(key);
    switch (key) {
    case 'p': mDispCurvePts = !mDispCurvePts; break;
    case 'c': mCpsCanvas.toggleVisibility(); break;
    case 'r': reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP)
    {
        mSpline.setNumPts(mSpline.getNumPts() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN)
    {
        mSpline.setNumPts(mSpline.getNumPts() - 1);
    }

    vvr_echo(mSpline.getNumPts());
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
