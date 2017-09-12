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

template <class ComponentT, size_t N, class CompositeT>
struct Composite : public vvr::Drawable
{
    static_assert(std::is_pointer<ComponentT>::value, "Components should be pointers");
    static_assert(N>1, "N must be 1+");

    std::array<ComponentT, N> components;
    CompositeT composite;

    template <typename... T>
    Composite(std::array<ComponentT, N> comp, vvr::Colour colour) 
        : components{ comp }
        , composite(assemble(components, std::make_index_sequence<N>(), colour))
    {
    }

    void addToCanvas(vvr::Canvas &canvas)
    {
        canvas.add(this);
        for (auto c : components) canvas.add(c);
    }

    template<typename Array, std::size_t... I>
    CompositeT assemble(const Array& a, std::index_sequence<I...>, vvr::Colour col) const
    {
        return CompositeT({ *a[I]... }, col);
    }

    template<typename Array, std::size_t... I>
    void update(const Array& a, std::index_sequence<I...>) const
    {
        const_cast<CompositeT&>(composite).setGeom({ *a[I]... });
    }

    void draw() const override
    {
        update(components, std::make_index_sequence<N>());
        composite.draw();
    }
};

struct Spline : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(Spline)

    bool disp_curve_pts = false;
    vvr::Colour colour;

    void addToCanvas(vvr::Canvas &canvas)
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
    typedef Composite<vvr::Point3D*, 3,  vvr::Triangle3D> CompositeTriangle;
    typedef Composite<vvr::Point3D*, 2, vvr::LineSeg3D> CompositeLine;
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

    /* Make drawables */
    auto spline = Spline::Make(vvr::red, vvr::red);

    auto lin = new CompositeLine({
        new vvr::Point3D(0,100,0, vvr::darkRed),
        new vvr::Point3D(0,200,0, vvr::darkRed)},
        vvr::darkRed);
    
    auto tri = new CompositeTriangle({
        new vvr::Point3D(-100,0,0, vvr::darkGreen),
        new vvr::Point3D(100,0,0, vvr::darkGreen),
        new vvr::Point3D(200,134,0, vvr::darkGreen)},
        vvr::darkGreen);

    /* Add to canvas */
    mCanvas.clear();
    spline->addToCanvas(mCanvas);
    lin->addToCanvas(mCanvas);
    tri->addToCanvas(mCanvas);

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

