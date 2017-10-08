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

struct Command
{
    virtual ~Command() { }
    virtual void operator()() = 0;
};

template <typename Receiver, typename Ret=void>
class SimpleCommand : public Command
{
    typedef Ret(Receiver::*Action)();
    Receiver* _receiver;
    Action _action;

public:
    SimpleCommand(Receiver* rec, Action action)
        : _receiver{rec}
        , _action{action}
    { }

    Ret operator()()
    {
        (_receiver->*_action)();
    }
};

class MacroCommand
{
    std::vector<Command*> _commands;

public:
    void add(Command* cmd) { _commands.push_back(cmd); }
    void operator()() { for (auto cmd : _commands) (*cmd)(); }
};

struct MouseInputConsumer
{
    virtual ~MouseInputConsumer() = 0;
    virtual bool consume(int x, int y, int modif) = 0;
};

using vvr::vec;

template <class T>
void snap2grid(T& x, T& y, T gs)
{
    const double dxn = floor(fabs((double)x / gs) + 0.5);
    const double dyn = floor(fabs((double)y / gs) + 0.5);
    x = (x < 0) ? (-dxn * gs) : (dxn * gs);
    y = (y < 0) ? (-dyn * gs) : (dyn * gs);
}

struct CurveBsp : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(CurveBsp)

    CurveBsp(vvr::Colour col_curve, vvr::Colour col_cps)
    {
        std::vector<vvr::Point3D*> cps;
        cps.push_back(new vvr::Point3D(-150, 100, 0, col_cps));
        cps.push_back(new vvr::Point3D(-50, -100, 0, col_cps));
        cps.push_back(new vvr::Point3D(50, 100, 0, col_cps));
        cps.push_back(new vvr::Point3D(150, -100, 0, col_cps));
        set_cps(cps);
        set_knots({ 0, 0, 0, 0, 1, 1, 1, 1 });
        set_num_pts(16);
        colour = col_curve;
    }

    void draw() const override
    {
        /* Draw curve and|or sample points. */
        for (auto it = get_pts().begin(); it < get_pts().end() - 1; ++it) {
            vvr::LineSeg3D(math::LineSegment(it[0], it[1]), colour).draw();
            if (drawCurvePts) it->draw();
        }
        if (drawCurvePts) get_pts().back().draw();
    }

    void addToCanvas(vvr::Canvas &canvas) override
    {
        canvas.add(this);
        for (auto cp : get_cps()) canvas.add(cp);
    }

    bool drawCurvePts = false;
    vvr::Colour colour;
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
    void append_to_grid(float dx, float dy, vvr::Colour);
    void save_scene();

    typedef vvr::CascadePicker2D<
        vvr::MousePicker2D<vvr::Point3D>,
        vvr::MousePicker2D<vvr::LineSeg3D>,
        vvr::MousePicker2D<vvr::Triangle3D>,
        vvr::MousePicker2D<vvr::Circle2D>,
        vvr::MousePicker2D<vvr::CompositeTriangle>,
        vvr::MousePicker2D<vvr::CompositeLine>
    > PickerT;

    int             _grid_size = 40;
    PickerT::Ptr    _picker;
    vvr::Canvas     _canvas;
    vvr::Canvas     _canvas_grid;
    CurveBsp*       _spline;
    vvr::Line2D*    _hl;
    vvr::Line2D*    _vl;

    MacroCommand _cmd_reset;
};

BSplineScene::BSplineScene()
{
    vvr::Shape::PointSize *= 2;
    m_bg_col = vvr::Colour("FFFFFF");
    reset();
    _cmd_reset.add(new SimpleCommand<BSplineScene>(this, &BSplineScene::reset));
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    _canvas.clear();

    /* Create objects */
    _spline = new CurveBsp(vvr::red, vvr::red);
    _spline->addToCanvas(_canvas);

    _hl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    _vl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    _hl->hide();
    _vl->hide();

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
    triangle->whole.filled = true;

    auto cir = new vvr::Circle2D(0, 0, 5);
    _canvas.add(cir);

    /* Create picker */
    _picker = PickerT::Make(_canvas);

    m_perspective_proj = false;
    setCameraPos({0,0,50});
}

void BSplineScene::resize()
{
    vvr::Colour col_1 ("CCCCCC");
    vvr::Colour col_2 ("666666");
    vvr::Colour col_3 ("000000");
    _canvas_grid.clear();
    append_to_grid((float)_grid_size,       (float)_grid_size,       col_1);
    append_to_grid((float)_grid_size * 10,  (float)_grid_size * 10,  col_2);
    append_to_grid((float)_grid_size * 100, (float)_grid_size * 100, col_3);
}

void BSplineScene::append_to_grid(float dx, float dy, vvr::Colour colour)
{
    const float sx = getViewportWidth();
    const float sy = getViewportHeight();
    const float nx = sx / dx;
    const float ny = sy / dy;
    const math::LineSegment lnx{ vec{ 0,-sy,0 }, vec{ 0,sy,0 } };
    const math::LineSegment lny{ vec{ -sx,0,0 }, vec{ sx,0,0 } };

    for (int i = -nx / 2; i <= nx / 2; i++) {
        auto l = lnx;
        l.Translate({ dx*i, 0, 0 });
        _canvas_grid.add(new vvr::LineSeg3D{ l, colour });
    }

    for (int i = -ny / 2; i <= ny / 2; i++) {
        auto l = lny;
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
    if (!shiftDown(modif)) {
        snap2grid(x, y, _grid_size);
    }
    _hl->set(x, y, x + 1, y);
    _vl->set(x, y, x, y + 1);
    _picker->drag(vvr::Mousepos{ x, y }, modif);
}

void BSplineScene::mouseReleased(int x, int y, int modif)
{
    _picker->drop();
}

void BSplineScene::mouseHovered(int x, int y, int modif)
{
    _picker->pick(vvr::Mousepos{ x, y }, 0);
    _hl->set(x, y, x + 1, y); 
    _vl->set(x, y, x, y + 1);
}

void BSplineScene::draw()
{    
    enterPixelMode();
    {
        _spline->update(true);
        _canvas_grid.drawif();
        _canvas.draw();
        _hl->drawif();
        _vl->drawif();
    }
    exitPixelMode();
}

void BSplineScene::save_scene()
{
    for (vvr::Drawable* drw : _canvas.getDrawables())
    {

        if (auto x = dynamic_cast<math::vec*>(drw))
        {
            vvr_msg(*x);
        }
        else if (auto x = dynamic_cast<math::Triangle*>(drw))
        {
            vvr_msg(*x);
        }
        else if (auto x = dynamic_cast<math::LineSegment*>(drw))
        {
            vvr_msg(*x);
        }

    }
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key)) {
    case 'p': _spline->drawCurvePts ^= true; break;
    case 'g': _canvas_grid.toggleVisibility(); break;
    case 's': if (ctrlDown(modif)) save_scene(); break;
    case ' ': _hl->toggleVisibility(); _vl->toggleVisibility(); break;
    case 'r': _cmd_reset(); break;
    }
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) _spline->set_num_pts(_spline->get_pts().size() + 1);
    else if (dir == vvr::ArrowDir::DOWN) _spline->set_num_pts(_spline->get_pts().size() - 1);
}

vvr_invoke_main_with_scene(BSplineScene)
