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

#define col_cps vvr::darkOrange

struct DrawableSpline : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(DrawableSpline)

    bool disp_curve_pts = true;

    void draw() const override
    {
        auto pts = getPts();
        for (auto it = pts.begin(); it < pts.end() - 1; ++it) {
            vvr::LineSeg3D(math::LineSegment(it[0], it[1])).draw();
            if (disp_curve_pts) it->draw();
        }
        if (disp_curve_pts) pts.back().draw();
    }
};

template <>
struct vvr::Dragger2D<vvr::Point3D, DrawableSpline>
{
    bool grab(vvr::Point3D* pt)
    {
        return _grabber.grab(pt);
    }

    void drag(int dx, int dy)
    {
        _grabber.drag(dx, dy);
        _spline->update(true);
    }

    void drop()
    {
        _grabber.drop();
    }

    Dragger2D(DrawableSpline *spline) : _spline(spline) {}

private:
    vvr::Dragger2D<vvr::Point3D> _grabber;
    DrawableSpline *_spline;
};

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

    typedef vvr::MousePicker2D<vvr::Point3D, DrawableSpline> picker_t;
    typedef vvr::Dragger2D<vvr::Point3D, DrawableSpline> dragger_t;

    picker_t::Ptr mPicker;
    vvr::Canvas mCanvas;
    DrawableSpline::Ptr mSpline;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 4;
    reset();
}

struct EditableLineSeg : public vvr::Drawable
{
    vvr::Point3D *p1, *p2;
    EditableLineSeg()
    {
        p1 = new vvr::Point3D(vvr::red);
        p2 = new vvr::Point3D(vvr::green);
    }
    void draw() const override
    {
        vvr::LineSeg3D(math::LineSegment(*p1, *p2)).drawif();
    }
};

void BSplineScene::reset()
{
    vvr::Scene::reset();
    mCanvas.clear();

    std::vector<vvr::Point3D*> cps;
    cps.push_back(new vvr::Point3D(-150,  100, 0, col_cps));
    cps.push_back(new vvr::Point3D( -50, -100, 0, col_cps));
    cps.push_back(new vvr::Point3D(  50,  100, 0, col_cps));
    cps.push_back(new vvr::Point3D( 150, -100, 0, col_cps));
    for (auto cp : cps) mCanvas.add(cp);
    mSpline = DrawableSpline::Make();
    mSpline->setCtrPts(cps);
    mSpline->setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
    mSpline->setNumPts(16);

    auto ln = new EditableLineSeg;
    ln->p1->set({0,100,0});
    ln->p2->set({0,200,0});
    mCanvas.add(ln);
    mCanvas.add(ln->p1);
    mCanvas.add(ln->p2);

    mPicker = picker_t::Make(&mCanvas, dragger_t(mSpline.get()));
}

void BSplineScene::draw()
{
    enterPixelMode();
    mSpline->drawif();
    mCanvas.drawif();
    exitPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    key = ::tolower(key);
    switch (key) {
    case 'c': for (auto cp : mSpline->getCtrlPts()) cp->toggleVisibility(); break;
    case 's': mSpline->toggleVisibility(); break;
    case 'p': mSpline->disp_curve_pts ^= true; break;
    case 'r': reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP)
    {
        mSpline->setNumPts(mSpline->getNumPts() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN)
    {
        mSpline->setNumPts(mSpline->getNumPts() - 1);
    }

    vvr_echo(mSpline->getNumPts());
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
