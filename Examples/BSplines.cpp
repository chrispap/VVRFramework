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

template <class ComponentT, class CompositeT, size_t N>
struct CompositeShape : public vvr::Drawable
{
    static_assert(N>1, "NumBlocks must be 1+");

    std::array<ComponentT, N> p;
    vvr::Colour colour;

    void draw() const override
    {
        update();
        b.draw();
    }

    template<typename Array, std::size_t... I>
    void update_impl(const Array& a, std::index_sequence<I...>) const
    {
        const_cast<CompositeT&>(b) = CompositeT({*a[I]...}, colour);
    }

    void update() const
    {
        return update_impl(p, std::make_index_sequence<N>());
    }

private:
    CompositeT b;
};

struct Spline : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(Spline)

    bool disp_curve_pts = false;
    vvr::Colour colour;

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
        cps.push_back(new vvr::Point3D(-150,  100, 0, col_cps));
        cps.push_back(new vvr::Point3D( -50, -100, 0, col_cps));
        cps.push_back(new vvr::Point3D(  50,  100, 0, col_cps));
        cps.push_back(new vvr::Point3D( 150, -100, 0, col_cps));
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

    Dragger2D(Spline *spline) : _spline(spline) {}

private:
    vvr::Dragger2D<vvr::Point3D> _grabber;
    Spline *_spline;
};

/*---[Scene]----------------------------------------------------------------------------*/

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
    typedef CompositeShape<vvr::Point3D*, vvr::Triangle3D, 3> CompositeTriangle;
    typedef CompositeShape<vvr::Point3D*, vvr::LineSeg3D, 2> CompositeLine;
    typedef vvr::MousePicker2D<vvr::Point3D, Spline> picker_t;
    typedef vvr::Dragger2D<vvr::Point3D, Spline> dragger_t;
    picker_t::Ptr mPicker;
    vvr::Canvas mCanvas;
    Spline* mSpline;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 2;
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    mCanvas.clear();

    /* Create & add to canvas: Line */
    auto lin = new CompositeLine;
    lin->p[0] = new vvr::Point3D(0,100,0, vvr::darkRed);
    lin->p[1] = new vvr::Point3D(0,200,0, vvr::darkRed);
    lin->colour = vvr::darkRed;
    mCanvas.add(lin);
    mCanvas.add(lin->p[0]);
    mCanvas.add(lin->p[1]);

    /* Create & add to canvas: Triangle */
    auto tri = new CompositeTriangle;
    tri->p[0] = new vvr::Point3D{-100,0,0, vvr::darkGreen};
    tri->p[1] = new vvr::Point3D{100,0,0, vvr::darkGreen};
    tri->p[2] = new vvr::Point3D{200,134,0, vvr::darkGreen};
    tri->colour = vvr::darkGreen;
    mCanvas.add(tri);
    mCanvas.add(tri->p[0]);
    mCanvas.add(tri->p[1]);
    mCanvas.add(tri->p[2]);

    /* Create & add to canvas: Spline */
    auto spline = Spline::Make(vvr::red, vvr::red);
    mCanvas.add(spline);
    for (auto cp : spline->getCps()) mCanvas.add(cp);

    /* Create a picker on the canvas */
    mSpline = spline;
    mPicker = picker_t::Make(&mCanvas, dragger_t(mSpline));
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
