#include <vvr/bspline.h>
#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/geom.h>
#include <vvr/picking.h>
#include <vvr/command.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <functional>

 /*---[Curve]---------------------------------------------------------------------------*/
struct CurveBsp4 : public vvr::BSpline<vvr::Point3D*>, public vvr::Drawable
{
    vvr_decl_shared_ptr(CurveBsp4)

public:
    CurveBsp4(const std::vector<vvr::Point3D*> &cps, vvr::Colour col_curve)
    {
        set_cps(cps);
        set_knots({ 0, 0, 0, 0, 1, 1, 1, 1 });
        set_num_pts(32);
        disp_pts = true;
        colour = col_curve;
    }

    void draw() const override
    {
        const_cast<CurveBsp4*>(this)->update(true);

        /* Draw curve and|or sample points. */
        for (auto it = get_pts().begin(); it < get_pts().end() - 1; ++it) {
            vvr::LineSeg3D(math::LineSegment(it[0], it[1]), colour).draw();
            if (disp_pts) it->draw();
        }
        if (disp_pts) get_pts().back().draw();
    }

    void addToCanvas(vvr::Canvas &canvas) override
    {
        canvas.add(this);
        for (auto cp : get_cps()) canvas.add(cp);
    }

public:
    bool disp_pts;
    vvr::Colour colour;
};

/*---[Sketcher]-------------------------------------------------------------------------*/
using vvr::vec;

class Sketcher : public vvr::Scene
{
public:
    Sketcher();

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
    void saveScene();

    typedef vvr::PriorityPicker2D<
        vvr::MousePicker2D<vvr::Point3D>,
        vvr::MousePicker2D<vvr::LineSeg3D>,
        vvr::MousePicker2D<vvr::Triangle3D>,
        vvr::MousePicker2D<vvr::Circle2D>,
        vvr::MousePicker2D<vvr::CompositeTriangle>,
        vvr::MousePicker2D<vvr::CompositeLine>
    > PickerT;

    int             m_gs = 40;
    PickerT::Ptr    m_picker;
    vvr::Canvas     m_canvas;
    vvr::Canvas     m_canvas_grid;
    CurveBsp4*      m_splines[2];
    vvr::Line2D*    m_hl;
    vvr::Line2D*    m_vl;
    std::vector<vvr::Point2D> m_p, m_h;

    vvr::MacroCommand m_cmd_reset;
};

Sketcher::Sketcher()
{
    vvr::Shape::PointSize *= 2;
    m_bg_col = vvr::Colour("FFFFFF");
    m_cmd_reset.add(new vvr::SimpleCommand<Sketcher>(this, &Sketcher::reset));
    m_canvas.setDelOnClear(false);
    reset();
}

void Sketcher::reset()
{
    vvr::Scene::reset();
    m_canvas.clear();
    m_p.clear();
    m_h.clear();

    /* Create objects */
    std::vector<vvr::Point3D*> cps;
    cps.push_back(new vvr::Point3D(250, 100, 0, vvr::red));
    cps.push_back(new vvr::Point3D( 50, 100, 0, vvr::red));
    cps.push_back(new vvr::Point3D( 20, 100, 0, vvr::red));
    cps.push_back(new vvr::Point3D(  0,   0, 0, vvr::red));
    m_splines[0] = new CurveBsp4(cps, vvr::red);
    m_splines[0]->addToCanvas(m_canvas);

    cps.clear();
    cps.push_back(m_splines[0]->get_cps().back());
    for (int i = m_splines[0]->get_cps().size()-2; i >= 0; i--) {
        cps.push_back(new vvr::Point3D(*m_splines[0]->get_cps()[i]));
        cps.back()->x *= -1;
        cps.back()->y *= -1;
    }
    m_splines[1] = new CurveBsp4(cps, vvr::red);
    m_splines[1]->addToCanvas(m_canvas);

    m_hl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    m_vl = new vvr::Line2D(0, 0, 0, 0, vvr::red);
    m_hl->hide();
    m_vl->hide();

    auto line = new vvr::CompositeLine({
        new vvr::Point3D(0, 100, 0, vvr::darkRed),
        new vvr::Point3D(100, 200, 0, vvr::darkRed) },
        vvr::darkRed);
    line->addToCanvas(m_canvas);

    auto triangle = new vvr::CompositeTriangle({
        new vvr::Point3D(0,0,0, vvr::darkGreen),
        new vvr::Point3D(300,0,0, vvr::darkGreen),
        new vvr::Point3D(200,150,0, vvr::darkGreen) },
        vvr::darkGreen);
    triangle->addToCanvas(m_canvas);
    triangle->whole.filled = true;

    auto cir = new vvr::Circle2D(0, 0, 55);
    m_canvas.add(cir);

    /* Create picker */
    m_picker = PickerT::Make(m_canvas);

    m_perspective_proj = false;
    setCameraPos({0,0,50});
}

void Sketcher::resize()
{
    vvr::Colour col_1 ("CCCCCC");
    vvr::Colour col_2 ("666666");
    vvr::Colour col_3 ("000000");
    m_canvas_grid.clear();
    append_to_grid((float)m_gs,       (float)m_gs,       col_1);
    append_to_grid((float)m_gs * 10,  (float)m_gs * 10,  col_2);
    append_to_grid((float)m_gs * 100, (float)m_gs * 100, col_3);
}

void Sketcher::append_to_grid(float dx, float dy, vvr::Colour colour)
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
        m_canvas_grid.add(new vvr::LineSeg3D{ l, colour });
    }

    for (int i = -ny / 2; i <= ny / 2; i++) {
        auto l = lny;
        l.Translate({ 0, dy*i, 0 });
        m_canvas_grid.add(new vvr::LineSeg3D{ l, colour });
    }
}

void Sketcher::mousePressed(int x, int y, int modif)
{
    m_picker->pick(vvr::Mousepos{ x, y }, modif);
}

void Sketcher::mouseMoved(int x, int y, int modif)
{
    if (m_canvas_grid.visible && !shiftDown(modif)) {
        vvr::snap_to_grid(x, y, m_gs);
    }
    m_hl->set(x, y, x + 1, y);
    m_vl->set(x, y, x, y + 1);

    if (m_picker->picked())
    {
        m_picker->drag(vvr::Mousepos{ x, y }, modif);
    }
    else
    {
        m_p.push_back({(float)x,(float)y});
        m_p = m_h = vvr::convex_hull(m_p);
    }
}

void Sketcher::mouseReleased(int x, int y, int modif)
{
    m_picker->drop();
}

void Sketcher::mouseHovered(int x, int y, int modif)
{
    m_picker->pick(vvr::Mousepos{ x, y }, 0);
    m_hl->set(x, y, x + 1, y);
    m_vl->set(x, y, x, y + 1);
}

void Sketcher::draw()
{
    enterPixelMode();
    m_canvas_grid.drawif();
    m_canvas.draw();
    m_hl->drawif();
    m_vl->drawif();

    for (int i=0; i<m_h.size(); i++) {
        int j = (i+1)%m_h.size();
        const auto &p = m_h[i];
        const auto &q = m_h[j];
        vvr::LineSeg2D(p.x, p.y, q.x, q.y, vvr::red).draw();
        p.draw();
    }

    exitPixelMode();
}

void Sketcher::saveScene()
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

void Sketcher::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key)) {
    case 'p': for (auto s:m_splines) s->disp_pts ^= true; break;
    case 'g': m_canvas_grid.toggleVisibility(); break;
    case 's': if (ctrlDown(modif)) saveScene(); break;
    case ' ': m_hl->toggleVisibility(); m_vl->toggleVisibility(); break;
    case 'r': m_cmd_reset(); break;
    }
}

void Sketcher::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) {
        m_splines[0]->set_num_pts(m_splines[0]->get_pts().size() + 1);
        m_splines[1]->set_num_pts(m_splines[0]->get_pts().size() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN) {
        m_splines[0]->set_num_pts(m_splines[0]->get_pts().size() - 1);
        m_splines[1]->set_num_pts(m_splines[0]->get_pts().size() - 1);
    }
    else if (dir == vvr::ArrowDir::LEFT) {
        if (m_gs==1) return;
        m_gs--;
        resize();
    }
    else if (dir == vvr::ArrowDir::RIGHT) {
        m_gs++;
        resize();
    }
}

vvr_invoke_main_with_scene(Sketcher)
