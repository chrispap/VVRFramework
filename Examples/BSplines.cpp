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

struct DrawableSpline : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(DrawableSpline)

    bool drawCurvePts = false;
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
            if (drawCurvePts) it->draw();
        }
        if (drawCurvePts) pts.back().draw();
    }

    static auto Make(vvr::Colour col_curve, vvr::Colour col_cps)
    {
        auto spline = new DrawableSpline;
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

class BSplineScene : public vvr::Scene
{
public:
    BSplineScene();

private:
    const char* getName() const override { return "BSpline Scene"; }
    void draw() override;
    void reset() override;
    void resize() override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override { _picker->pick(x, y, modif); }
    void mouseMoved(int x, int y, int modif) override { _picker->drag(x, y, modif); }
    void mouseReleased(int x, int y, int modif) override { _picker->drop(x, y, modif); }
    void mouseHovered(int x, int y, int modif) override { _picker->pick(x, y, modif); }
    void appendGrid(float dx, float dy, vvr::Colour);

    typedef vvr::MousePicker2D<vvr::Point3D>                    PickerT1;
    typedef vvr::MousePicker2D<vvr::CompositeTriangle>          PickerT2;
    typedef vvr::MousePicker2D<vvr::CompositeLine>              PickerT3;
    typedef vvr::CascadePicker2D<PickerT1, PickerT2,PickerT3>   PickerT;
    typedef vvr::MousePicker2D<vvr::LineSeg3D>                  HlterT;

    PickerT::Ptr    _picker;
    HlterT::Ptr     _hlter;
    DrawableSpline* _spline;
    vvr::Canvas     _canvas;
    vvr::Canvas     _canvas_grid;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 2;
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    _canvas.clear();

    /* Create objects */
    _spline = DrawableSpline::Make(vvr::red, vvr::red);
    _spline->addToCanvas(_canvas);

    auto line = new vvr::CompositeLine({
        new vvr::Point3D(0, 100, 0, vvr::darkRed),
        new vvr::Point3D(100, 200, 0, vvr::darkRed) },
        vvr::darkRed);
    line->addToCanvas(_canvas);

    auto triangle = new vvr::CompositeTriangle({
        new vvr::Point3D(0,0,0, vvr::darkGreen),
        new vvr::Point3D(300,0,0, vvr::darkGreen),
        new vvr::Point3D(200,150,0, vvr::darkGreen) },
        vvr::darkGreen);
    triangle->addToCanvas(_canvas);

    /* Create picker */
    _picker = PickerT::Make(_canvas);

    /* Create hlter */
    _hlter = HlterT::Make(_canvas_grid);
}

void BSplineScene::resize()
{
    vvr::Colour col_1 = "EEEEEE";
    vvr::Colour col_2 = "666666";
    vvr::Colour col_3 = "000000";
    _canvas_grid.clear();
    appendGrid(10, 10, col_1);
    appendGrid(100, 100, col_2);
    appendGrid(1000, 1000, col_3);
}

void BSplineScene::appendGrid(float dx, float dy, vvr::Colour colour)
{
    const float sx = getViewportWidth();
    const float sy = getViewportHeight();
    const float nx = sx / dx;
    const float ny = sy / dy;
    const math::LineSegment lnx{ vec{ 0,-sy,0 }, vec{ 0,sy,0 } };
    const math::LineSegment lny{ vec{ -sx,0,0 }, vec{ sx,0,0 } };

    for (int i = -nx / 2; i <= nx / 2; i++) {
        auto l{ lnx };
        l.Translate({ dx*i, 0, 0 });
        _canvas_grid.add(new vvr::LineSeg3D{ l, colour });
    }

    for (int i = -ny / 2; i <= ny / 2; i++) {
        auto l{ lny };
        l.Translate({ 0, dy*i, 0 });
        _canvas_grid.add(new vvr::LineSeg3D{ l, colour });
    }
}

void BSplineScene::draw()
{
    enterPixelMode();
    _spline->update(true);
    _canvas_grid.drawif();
    _canvas.draw();
    exitPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key)) {
    case 's': _spline->toggleVisibility(); break;
    case 'p': _spline->drawCurvePts ^= true; break;
    case 'g': _canvas_grid.toggleVisibility(); break;
    case 'r': reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) _spline->setNumPts(_spline->getNumPts() + 1);
    else if (dir == vvr::ArrowDir::DOWN) _spline->setNumPts(_spline->getNumPts() - 1);
}

int main(int argc, char* argv[])
{
    try { return vvr::mainLoop(argc, argv, new BSplineScene); }
    catch (std::string exc) { std::cerr << exc << std::endl; return 1; }
}
