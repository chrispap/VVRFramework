#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/animation.h>
#include <vvr/command.h>
#include <vvr/picking.h>
#include <vvr/geom.h>
#include <MathGeoLib.h>

using vvr::vec;
using vvr::real;

/*---[Functions]------------------------------------------------------------------------*/
static void smoothen(vvr::Canvas &sketch)
{
    if (!sketch.size())
        return;

    for (int i=0; i<1; ++i) {
        auto &drw = sketch.getDrawables(i);
        if (drw.size() < 3) continue;

        for (size_t i=1; i<drw.size()-1; ++i) {
            auto ls0 = static_cast<vvr::LineSeg3D*>(drw.at(i-1));
            auto ls1 = static_cast<vvr::LineSeg3D*>(drw.at(i));
            auto ls2 = static_cast<vvr::LineSeg3D*>(drw.at(i+1));
            ls1->a = (ls0->a + ls1->a + ls2->a) / 3;
        }

        for (unsigned i=0; i<drw.size()-1; ++i) {
            auto ls1 = static_cast<vvr::LineSeg3D*>(drw.at(i));
            auto ls2 = static_cast<vvr::LineSeg3D*>(drw.at(i + 1));
            ls1->b = ls2->a;
        }
    }
}

static void append(vvr::Canvas &sketch, const vec &p, vvr::Colour col)
{
    std::vector<vvr::Drawable*> &drw = sketch.getDrawables();

    if (drw.size()) {
        auto ls_prev = static_cast<vvr::LineSeg3D*>(drw.back());
        if (ls_prev->Distance(p) > 0.01) {
            sketch.add(new vvr::LineSeg3D({ls_prev->b, p}, col));
        }
    } else sketch.add(new vvr::LineSeg3D(p.x, p.y, p.z, p.x, p.y, p.z, col));
}

static void recolour(vvr::Canvas &sketch, vvr::Colour colour)
{
    for (auto d : sketch.getDrawables()) {
        static_cast<vvr::Shape*>(d)->colour = colour;
    }
}

/*---[Paper]----------------------------------------------------------------------------*/
struct Paper : vvr::Drawable
{
    vvr_decl_shared_ptr(Paper)

    Paper(float w, float h, const float3x4 &sys=math::float3x4::identity);
    void draw() const override;
    real pickdist(const Ray &) const override;
    vec intersect(math::Ray &&ray) const;
    void toggleFill() { disp_fill ^= true; }
    void toggleWire() { disp_wire ^= true; }
    void togglePts() { disp_pts ^= true; }
    const math::Polygon& getPolygon() const { return pol; }
    math::float2 getSize() { return {w*2, h*2}; }

private:
    void update();
    void setLocsys(const float3x4 &aSys);

private:
    real w,h;
    math::float3x4 sys;
    math::Polygon pol;
    math::TriangleArray tris;
    vvr::Colour colour;
    bool disp_fill = true;
    bool disp_wire = true;
    bool disp_pts = true;
};

Paper::Paper(float w, float h, const math::float3x4& sys) : w(w), h(h), sys(sys)
{
    colour = {"F0E8B6"};
    update();
}

void Paper::setLocsys(const float3x4 &aSys)
{
    sys = aSys;
    update();
}

void Paper::update()
{
    pol.p.clear();
    pol.p.reserve(4);
    pol.p.push_back(vec(-w, +h, 0));
    pol.p.push_back(vec(+w, +h, 0));
    pol.p.push_back(vec(+w, -h, 0));
    pol.p.push_back(vec(-w, -h, 0));
    pol.Transform(sys);
    tris = pol.Triangulate();
}

real Paper::pickdist(const Ray &ray) const
{
    if (!ray.Intersects(pol)) return -1;
    else return pol.Distance(ray.pos);
}

vec Paper::intersect(math::Ray &&ray) const
{
    if (!pol.Intersects(ray)) return vec::inf;
    return pol.ClosestPoint(ray.ToLineSegment(10000));
}

void Paper::draw() const
{
    if (disp_fill) {
        for (auto &tri : tris) {
            vvr::Triangle3D(tri, colour).draw();
        }
    }

    if (disp_wire) {
        for (int i=0; i < pol.NumEdges(); ++i) {
            vvr::LineSeg3D(pol.Edge(i), vvr::black).draw();
        }
    }

    if (disp_pts) {
        for (int i=0; i < pol.NumVertices(); ++i) {
            vvr::Point3D(pol.Vertex(i), vvr::black).draw();
        }
    }
}

/*---[PaperDragger]---------------------------------------------------------------------*/
struct PaperDragger
{
    vvr_decl_shared_ptr(PaperDragger)
    PaperDragger(vvr::Canvas &sketch);
    bool on_pick(vvr::Drawable* drw, Ray ray);
    void on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1);
    void on_drop(vvr::Drawable* drw);
    void UpdateCrosshair(Paper *paper, const vec &p);
    void HideCrosshair();
    vvr::Canvas &sketch;
    vvr::LineSeg3D hl,vl;
};

PaperDragger::PaperDragger(vvr::Canvas &sketch) : sketch(sketch)
{
    hl.colour = vvr::Gray;
    vl.colour = vvr::Gray;
    hl.hide();
    vl.hide();
}

void PaperDragger::HideCrosshair()
{
    hl.hide();
    vl.hide();
}

void PaperDragger::UpdateCrosshair(Paper *paper, const vec& p)
{
    auto pol = paper->getPolygon();
    math::float2 uv = pol.MapTo2D(p);
    math::float2 sz = paper->getSize();
    hl.a = pol.MapFrom2D({    0, uv[1]});
    hl.b = pol.MapFrom2D({sz[0], uv[1]});
    vl.a = pol.MapFrom2D({uv[0],    0});
    vl.b = pol.MapFrom2D({uv[0], sz[1]});
    hl.show();
    vl.show();
}

bool PaperDragger::on_pick(vvr::Drawable *drw, Ray ray)
{
    auto paper = static_cast<Paper*>(drw);
    vec ip = paper->intersect(std::move(ray));
    UpdateCrosshair(paper, ip);
    return true;
}

void PaperDragger::on_drag(vvr::Drawable *drw, Ray ray0, Ray ray1)
{
    auto paper = static_cast<Paper*>(drw);
    const vec ip = paper->intersect(std::move(ray1));
    if (ip.IsFinite()) append(sketch, ip, vvr::LightCoral);
}

void PaperDragger::on_drop(vvr::Drawable *drw)
{
    smoothen(sketch);
    smoothen(sketch);
    recolour(sketch, vvr::red);
    sketch.newFrame();
}

/*---[OrigamiScene]---------------------------------------------------------------------*/
class OrigamiScene : public vvr::Scene
{
public:
    OrigamiScene();

private:
    const char* getName() const override { return "Origami Scene"; }
    void mouseHovered(int x, int y, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void draw() override;

private:
    typedef vvr::MousePicker3D<PaperDragger> PickerT;

    Paper::Ptr          m_paper;
    vvr::Canvas         m_papers;
    vvr::Canvas         m_sketch;
    vvr::Canvas         m_hull;
    vvr::KeyMap         m_keymap;
    PickerT::Ptr        m_picker;
    PaperDragger::Ptr   m_dragger;
};

OrigamiScene::OrigamiScene()
{
    m_bg_col = vvr::grey;
    m_perspective_proj = true;

    /* Create drawables and populate canvas. */
    float a = 10;
    m_paper = Paper::Make(a*math::Sqrt(2), a);
    m_papers.add(m_paper.get());
    m_papers.setDelOnClear(false);
    m_dragger = PaperDragger::Make(m_sketch);
    m_picker = PickerT::Make(m_papers, m_dragger.get());

    /* Install key bindings */
    m_keymap['a'].add(new vvr::SimpleCmd<vvr::Axes, bool>(&getGlobalAxes(), &vvr::Axes::toggle));
    m_keymap['x'].add(new vvr::SimpleCmd<vvr::Canvas>(&m_sketch, &vvr::Canvas::clear));
    m_keymap['x'].add(new vvr::SimpleCmd<vvr::Canvas>(&m_hull, &vvr::Canvas::clear));
    m_keymap['h'].add(new vvr::SimpleCmd<vvr::Canvas,bool>(&m_hull, &vvr::Canvas::hide));
    m_keymap['s'].add(new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::toggleFill));
    m_keymap['w'].add(new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::toggleWire));
    m_keymap['p'].add(new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::togglePts));

    /* Default visibility */
    getGlobalAxes().hide();
}

void OrigamiScene::draw()
{
    getGlobalAxes().drawif();
    m_paper->drawif();
    m_sketch.drawif();
    m_hull.drawif();
    m_dragger->hl.drawif();
    m_dragger->vl.drawif();
}

void OrigamiScene::mouseHovered(int x, int y, int modif)
{
    m_picker->do_pick(unproject(x,y), modif);
    if (m_picker->picked()) {
        vvr::set_status_bar_msg("Sketch on paper surface");
        cursorHide();
    } else {
        vvr::clear_status_bar();
        m_dragger->HideCrosshair();
        cursorShow();
    }
}

void OrigamiScene::mousePressed(int x, int y, int modif)
{
    auto ray = unproject(x,y);
    m_picker->do_drop(ray, modif);
    m_picker->do_pick(ray, modif);
    if (m_picker->picked()) {
        m_dragger->HideCrosshair();
        cursorHide();
    } else Scene::mousePressed(x, y, modif);
}

void OrigamiScene::mouseMoved(int x, int y, int modif)
{
    if (m_picker->picked()) {
        m_picker->do_drag(unproject(x,y), modif);
    } else Scene::mouseMoved(x, y, modif);
}

void OrigamiScene::mouseReleased(int x, int y, int modif)
{
    mouseHovered(x, y, modif);

    /* Compute/draw convex hull of latest sketch. */
    auto drw = m_sketch.getDrawables(-1);
    vec u = m_paper->getPolygon().BasisU();
    vec v = m_paper->getPolygon().BasisV();
    std::vector<math::float2> pts, ch;

    for (auto d : drw) {
        math::float2 p2;
        vvr::LineSeg3D* ls3;
        if (!(ls3 = dynamic_cast<vvr::LineSeg3D*>(d))) continue;
        p2 = math::float2(ls3->a.Dot(u), ls3->a.Dot(v)); pts.push_back(p2);
        p2 = math::float2(ls3->b.Dot(u), ls3->b.Dot(v)); pts.push_back(p2);
    }

    m_hull.newFrame();
    ch = vvr::convex_hull(pts);
    if (ch.size()<3) return;
    for (auto p: ch) append (m_hull, u*p.x + v*p.y, vvr::Blue);
    append(m_hull, u*ch.front().x + v*ch.front().y, vvr::Blue);
}

void OrigamiScene::mouseWheel(int dir, int modif)
{
    if (shiftDown(modif)) {
        vvr::Shape::LineWidth += 0.2f*dir;
    } else Scene::mouseWheel(dir, modif);
}

void OrigamiScene::keyEvent(unsigned char key, bool up, int modif)
{
    if (m_keymap.find(key) != std::end(m_keymap)) {
        m_keymap[key]();
    } else Scene::keyEvent(key, up, modif);
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(OrigamiScene)
#endif
