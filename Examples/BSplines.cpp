#include <vvr/bspline.h>
#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <functional>

struct Spline : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(Spline)

    bool disp_curve_pts = false;
    vvr::Colour colour;

    void addToCanvas(vvr::Canvas &canvas) override
    {
        canvas.add(this);
        for (auto cp : getCps()) canvas.add(cp);
    }

    void draw() const override
    {
        /* Draw curve and|or sample points. */
        auto pts(getPts());
        for (auto it = pts.begin(); it < pts.end() - 1; ++it) {
            vvr::LineSeg3D(math::LineSegment(it[0], it[1]), colour).draw();
            if (disp_curve_pts) it->draw();
        }
        if (disp_curve_pts) pts.back().draw();
    }

    static auto Make(vvr::Colour col_curve, vvr::Colour col_cps)
    {
        auto spline = new Spline;
        std::vector<vvr::Point3D*> cps;
        cps.push_back(new vvr::Point3D(-150, 100, 0, col_cps));
        cps.push_back(new vvr::Point3D(-50, -100, 0, col_cps));
        cps.push_back(new vvr::Point3D(50, 100, 0, col_cps));
        cps.push_back(new vvr::Point3D(150, -100, 0, col_cps));
        spline->setCtrPts(cps);
        spline->setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
        spline->setNumPts(16);
        spline->colour = col_curve;
        return spline;
    }
};

template <>
struct vvr::Dragger2D<vvr::Point3D, Spline>
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

    void setSpline(Spline *spline) { _spline = spline; }

private:
    vvr::Dragger2D<vvr::Point3D> _grabber;
    Spline *_spline=nullptr;
};

class BSplineScene : public vvr::Scene
{
public:
    BSplineScene();
    const char* getName() const override { return "BSpline Scene"; }
    void draw() override;
    void reset() override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override { mPicker->pick(x,y,modif); }
    void mouseMoved(int x, int y, int modif) override { mPicker->move(x,y,modif);}
    void mouseReleased(int x, int y, int modif) override {mPicker->drop(x,y,modif); }

private:
    typedef vvr::CascadePicker2D<
        vvr::MousePicker2D<vvr::Point3D, Spline>,
        vvr::MousePicker2D<vvr::CompositeTriangle>,
        vvr::MousePicker2D<vvr::CompositeLine>
    > picker_t;

    picker_t::Ptr mPicker;
    vvr::Canvas mCanvas;
    Spline* mSpline;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 3;
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    mCanvas.clear();

    (new vvr::CompositeLine({
        {new vvr::Point3D(0,100,0, vvr::darkRed),
         new vvr::Point3D(0,200,0, vvr::darkRed)} },
        vvr::darkRed))->addToCanvas(mCanvas);

    (new vvr::CompositeTriangle({
        {new vvr::Point3D(-100,0,0, vvr::darkGreen),
         new vvr::Point3D(100,0,0, vvr::darkGreen),
         new vvr::Point3D(200,134,0, vvr::darkGreen)} },
        vvr::darkGreen))->addToCanvas(mCanvas);

    mSpline = Spline::Make(vvr::red, vvr::red);
    mSpline->addToCanvas(mCanvas);

    /* Create picker */
    mPicker = picker_t::Make(mCanvas);
    auto &spline_dragger = std::get<vvr::MousePicker2D<vvr::Point3D, Spline>>(mPicker->pickers).getDragger();
    spline_dragger.setSpline(mSpline);
}

void BSplineScene::draw()
{
    enterPixelMode();
    mCanvas.draw();
    exitPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key)) {
    case 's': mSpline->toggleVisibility(); break;
    case 'p': mSpline->disp_curve_pts ^= true; break;
    case 'r': reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) mSpline->setNumPts(mSpline->getNumPts() + 1);
    else if (dir == vvr::ArrowDir::DOWN) mSpline->setNumPts(mSpline->getNumPts() - 1);
}

int main(int argc, char* argv[])
{
    try { return vvr::mainLoop(argc, argv, new BSplineScene); }
    catch (std::string exc) { std::cerr << exc << std::endl; return 1; }
}
