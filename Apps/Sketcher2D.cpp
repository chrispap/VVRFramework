#include <vvr/bspline.h>
#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/palette.h>
#include <vvr/geom.h>
#include <vvr/picking.h>
#include <vvr/command.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <functional>

namespace vvr
{
    /*---[Curve3D]----------------------------------------------------------------------*/
    template<typename T>
    struct Curve3D : public Drawable
    {
        typedef T curve_t;
        typedef typename curve_t::point_t point_t;

        Colour colour;
        bool disp_pts;

        Curve3D(curve_t &curve) : crv(curve) {}

        void Update(size_t num_pts) const
        {
            if (num_pts<2) num_pts = 2;
            auto range = crv.ParamRange();
            auto dt = (range.second - range.first) / (num_pts - 1);
            pts.resize(num_pts);
            for (size_t i = 0; i < num_pts; i++) {
                pts[i] = crv.Eval(range.first + dt * i);
            }
        }

        void draw() const override
        {
            Update(32);
            for (auto it = pts.begin(); it < pts.end() - 1; ++it) {
                LineSeg3D(math::LineSegment(it[0], it[1]), colour).draw();
                if (disp_pts) it->draw();
            }
            if (disp_pts) pts.back().draw();
        }

    private:
        mutable std::vector<point_t> pts;
        curve_t &crv;
    };

    /*---[CurveBsp]---------------------------------------------------------------------*/
    struct CurveBsp : Curve3D<BSpline<Point3D*> >
    {
        curve_t bsp;

        CurveBsp() : Curve3D<curve_t>(bsp) {}

        void addToCanvas(Canvas &canvas) override
        {
            canvas.add(this);
            for (auto cp : bsp.cps) {
                canvas.add(cp);
            }
        }
    };
}

/*---[Sketcher]-------------------------------------------------------------------------*/
class Sketcher : public vvr::Scene
{
public:
    Sketcher();

private:
    const char* getName() const override { return "2D Sketcher"; }
    void draw() override;
    void reset() override;
    void resize() override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseHovered(int x, int y, int modif) override;
    void add_spacing_to_grid(float dx, float dy, vvr::Colour);
    void make_grid();
    void save_scene();
    void toggle_points();
    void toggle_grid();
    void toggle_croshair();

    typedef vvr::PriorityPicker2D<
    /*0*/vvr::MousePicker2D<vvr::Point3D>,
    /*1*/vvr::MousePicker2D<vvr::LineSeg3D>,
    /*2*/vvr::MousePicker2D<vvr::Circle2D>,
    /*3*/vvr::MousePicker2D<vvr::CompositeTriangle>,
    /*4*/vvr::MousePicker2D<vvr::CompositeLine>
    > PickerT;

    int             m_gs = 40;
    PickerT::Ptr    m_picker;
    vvr::Canvas     m_canvas;
    vvr::Canvas     m_grid;
    vvr::Line2D*    m_hl;
    vvr::Line2D*    m_vl;
    vvr::CurveBsp*  m_bsps[2];

    std::unordered_map<char, vvr::MacroCmd> m_key_map;
};

/*--------------------------------------------------------------------------------------*/
Sketcher::Sketcher()
{
    vvr::Shape::PointSize *= 2;
    m_bg_col = vvr::Colour("FFFFFF");
    m_perspective_proj = false;
    m_canvas.setDelOnClear(false);

    /* Create keyboard mapping */
    m_key_map['p'].add((new vvr::SimpleCmd<Sketcher>(this, &Sketcher::toggle_points)));
    m_key_map['g'].add((new vvr::SimpleCmd<Sketcher>(this, &Sketcher::toggle_grid)));
    m_key_map['s'].add((new vvr::SimpleCmd<Sketcher>(this, &Sketcher::save_scene)));
    m_key_map['v'].add((new vvr::SimpleCmd<Sketcher>(this, &Sketcher::toggle_croshair)));

    /* Create picker */
    m_picker = PickerT::Make(m_canvas);
    m_picker->get<0>().dragger().col_hover = vvr::Blue;
    reset();
}

void Sketcher::reset()
{
    vvr::Scene::reset();

    m_picker->drop();
    m_canvas.clear();

    //! Create bsplines
    m_bsps[0] = new vvr::CurveBsp();
    m_bsps[1] = new vvr::CurveBsp();

    m_bsps[0]->bsp.knots = { 0, 0, 0, 0, 1, 1, 1, 1 };
    m_bsps[0]->bsp.cps.push_back(new vvr::Point3D(250, 100, 0, vvr::red));
    m_bsps[0]->bsp.cps.push_back(new vvr::Point3D( 50, 100, 0, vvr::red));
    m_bsps[0]->bsp.cps.push_back(new vvr::Point3D( 20, 100, 0, vvr::red));
    m_bsps[0]->bsp.cps.push_back(new vvr::Point3D(  0,   0, 0, vvr::red));

    m_bsps[1]->bsp.knots = m_bsps[0]->bsp.knots;
    m_bsps[1]->bsp.cps.push_back(m_bsps[0]->bsp.cps.back());
    for (int i = m_bsps[0]->bsp.cps.size()-2; i >= 0; i--) {
        m_bsps[1]->bsp.cps.push_back(new vvr::Point3D(*m_bsps[0]->bsp.cps[i]));
        m_bsps[1]->bsp.cps.back()->x *= -1;
        m_bsps[1]->bsp.cps.back()->y *= -1;
    }

    //! Create croshair lines.
    m_hl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    m_vl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    m_hl->hide();
    m_vl->hide();

    //! Create composite line
    auto line = new vvr::CompositeLine({
        {new vvr::Point3D(0, 100, 0, vvr::darkRed),
        new vvr::Point3D(100, 200, 0, vvr::darkRed)} },
        vvr::darkRed);

    //! Create composite triangle
    auto triangle = new vvr::CompositeTriangle({{
        new vvr::Point3D(0,0,0, vvr::darkGreen),
        new vvr::Point3D(300,0,0, vvr::darkGreen),
        new vvr::Point3D(200,150,0, vvr::darkGreen) }},
        vvr::darkGreen);
    triangle->whole.filled = true;

    auto cir = new vvr::Circle2D(0, 0, 55);

    //! Add to canvas.
    m_canvas.add(cir);
    line->addToCanvas(m_canvas);
    triangle->addToCanvas(m_canvas);
    m_bsps[0]->addToCanvas(m_canvas);
    m_bsps[1]->addToCanvas(m_canvas);

    setCameraPos({0,0,50});
}

void Sketcher::resize()
{
    static bool first_pass = true;
    make_grid();
    first_pass = false;
}

void Sketcher::mouseHovered(int x, int y, int modif)
{
    m_hl->set(x, y, x + 1, y);
    m_vl->set(x, y, x, y + 1);
    m_picker->pick(vvr::Mousepos{ x, y }, 0, false);
    if (m_picker->picked()) {
        cursorHand();
    } else cursorShow();
}

void Sketcher::mousePressed(int x, int y, int modif)
{
    m_picker->pick(vvr::Mousepos{ x, y }, modif, true);
    if (m_picker->picked()) cursorGrab();
}

void Sketcher::mouseMoved(int x, int y, int modif)
{
    if (m_grid.visible && !shiftDown(modif)) {
        vvr::snap_to_grid(x, y, m_gs);
    }
    {
        m_hl->set(x, y, x + 1, y);
        m_vl->set(x, y, x, y + 1);
    }
    if (m_picker->picked()) {
        m_picker->drag(vvr::Mousepos{ x, y }, modif);
    }
}

void Sketcher::mouseReleased(int x, int y, int modif)
{
    if (m_picker->picked()) cursorShow();
    m_picker->drop();
}

void Sketcher::keyEvent(unsigned char key, bool up, int modif)
{
    unsigned char k = tolower(key);
    if (m_key_map.find(k) != m_key_map.end()) {
        m_key_map[k]();
    } else Scene::keyEvent(key, up, modif);
}

void Sketcher::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::LEFT) {
        if (m_gs>1) {
            m_gs--;
            resize();
        }
    }
    else if (dir == vvr::ArrowDir::RIGHT) {
        m_gs++;
        resize();
    }
}

void Sketcher::draw()
{
    enterPixelMode();
    {
        m_grid.drawif();
        m_canvas.draw();
        if (auto p=m_picker->picked()) {
            p->draw();
        }
        vvr::real lw = vvr::Shape::SetLineWidth(1);
        m_hl->drawif();
        m_vl->drawif();
        lw = vvr::Shape::SetLineWidth(lw);
    }
    exitPixelMode();
}

void Sketcher::make_grid()
{
    m_grid.clear();
    vvr::Colour col_1 ("CCCCCC");
    vvr::Colour col_2 ("666666");
    vvr::Colour col_3 ("000000");
    add_spacing_to_grid((float)m_gs,       (float)m_gs,       col_1);
    add_spacing_to_grid((float)m_gs * 10,  (float)m_gs * 10,  col_2);
    add_spacing_to_grid((float)m_gs * 100, (float)m_gs * 100, col_3);
}

void Sketcher::add_spacing_to_grid(float dx, float dy, vvr::Colour colour)
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
        m_grid.add(new vvr::LineSeg3D{ l, colour });
    }

    for (int i = -ny / 2; i <= ny / 2; i++) {
        auto l = lny;
        l.Translate({ 0, dy*i, 0 });
        m_grid.add(new vvr::LineSeg3D{ l, colour });
    }
}

void Sketcher::save_scene()
{
    for (vvr::Drawable* drw : m_canvas.getDrawables())
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

void Sketcher::toggle_points()
{
    for (auto s : m_bsps) {
        s->disp_pts = !s->disp_pts;
    }
}

void Sketcher::toggle_grid()
{
    m_grid.toggleVisibility();
}

void Sketcher::toggle_croshair()
{
    m_hl->toggleVisibility();
    m_vl->toggleVisibility();
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(Sketcher)
#endif
