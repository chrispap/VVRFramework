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

template <class T>
void snap2grid(T& x, T& y, T gs)
{
    const double dxn = floor(fabs((double)x / gs) + 0.5);
    const double dyn = floor(fabs((double)y / gs) + 0.5);
    x = (x < 0) ? (-dxn * gs) : (dxn * gs);
    y = (y < 0) ? (-dyn * gs) : (dyn * gs);
}

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
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseHovered(int x, int y, int modif) override;
    void append2Grid(float dx, float dy, vvr::Colour);

    typedef vvr::CascadePicker2D<
        vvr::MousePicker2D<vvr::Point3D>,
        vvr::MousePicker2D<vvr::LineSeg3D>,
        vvr::MousePicker2D<vvr::Triangle3D>,
        vvr::MousePicker2D<vvr::Circle2D>,
        vvr::MousePicker2D<vvr::CompositeTriangle>,
        vvr::MousePicker2D<vvr::CompositeLine>
    > PickerT;

    PickerT::Ptr    _picker;
    DrawableSpline* _spline;
    vvr::Canvas     _canvas;
    vvr::Canvas     _canvas_grid;
    int             _grid_size = 40;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 2;
    m_bg_col = vvr::Colour("EEEEEE");
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
    triangle->whole.filled = false;

    _canvas.add(new vvr::Triangle3D(triangle->whole));

    _canvas.add(new vvr::LineSeg3D(math::LineSegment(vec(110,110,0), vec(333,50,0)), vvr::orange));

    _canvas.add(new vvr::Circle2D(0,0, 200));

    /* Create picker */
    _picker = PickerT::Make(_canvas);
}

void BSplineScene::resize()
{
    vvr::Colour col_1 = "CCCCCC";
    vvr::Colour col_2 = "666666";
    vvr::Colour col_3 = "000000";
    _canvas_grid.clear();
    append2Grid(_grid_size, _grid_size, col_1);
    append2Grid(_grid_size * 10, _grid_size * 10, col_2);
    append2Grid(_grid_size * 100, _grid_size * 100, col_3);
}

void BSplineScene::append2Grid(float dx, float dy, vvr::Colour colour)
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

void BSplineScene::mousePressed(int x, int y, int modif)
{
    _picker->pick(vvr::Mousepos{ x, y }, modif);
}

void BSplineScene::mouseMoved(int x, int y, int modif)
{
    if (shiftDown(modif)) {
        snap2grid(x, y, _grid_size);
    }
    _picker->drag(vvr::Mousepos{ x, y }, modif);
}

void BSplineScene::mouseReleased(int x, int y, int modif)
{
    _picker->drop();
}

void BSplineScene::mouseHovered(int x, int y, int modif)
{
    _picker->pick(vvr::Mousepos{ x, y }, 0);
}

void BSplineScene::draw()
{
    enterPixelMode();
    {
        _spline->update(true);
        _canvas_grid.drawif();
        _canvas.draw();
    }
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
