#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/animation.h>
#include <vvr/command.h>
#include <vvr/picking.h>
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

static void append(vvr::Canvas &sketch, const vec &p)
{
    const vvr::Colour &col = vvr::red;
    std::vector<vvr::Drawable*> &drw = sketch.getDrawables();

    if (drw.size()) {
        auto ls_prev = static_cast<vvr::LineSeg3D*>(drw.back());
        if (ls_prev->Distance(p) > 0.01) {
            sketch.add(new vvr::LineSeg3D({ls_prev->b, p}, col));
        }
    } else sketch.add(new vvr::LineSeg3D(p.x, p.y, p.z, p.x, p.y, p.z, col));
}

/*---[Paper]----------------------------------------------------------------------------*/
struct Paper : vvr::Drawable
{
    vvr_decl_shared_ptr(Paper)

    Paper(float w, float h);
    void draw() const override;
    real pickdist(const Ray &) const override;
    vec Intersects(math::Ray &&ray) const;
    void toggleFill() { disp_fill ^= true; }
    void toggleWire() { disp_wire ^= true; }
    void togglePts() { disp_pts ^= true; }

private:
    math::TriangleArray tris;
    math::Polygon pol;
    math::float4x4 mat = math::float4x4::identity;
    vvr::Colour col = vvr::Colour("F0E8B6");
    bool disp_fill = true;
    bool disp_wire = true;
    bool disp_pts = true;
};

Paper::Paper(float w, float h)
{
    pol.p.push_back(math::vec(-w, -h, 0));
    pol.p.push_back(math::vec(-w, +h, 0));
    pol.p.push_back(math::vec(+w, +h, 0));
    pol.p.push_back(math::vec(+w, -h, 0));
    pol.Transform(mat);
    tris = pol.Triangulate();
}

real Paper::pickdist(const Ray &ray) const
{
    if (!ray.Intersects(pol)) return -1;
    else return pol.Distance(ray.pos);
}

vec Paper::Intersects(math::Ray &&ray) const
{
    if (!pol.Intersects(ray)) return vec::inf;
    return pol.ClosestPoint(ray.ToLineSegment(10000));
}

void Paper::draw() const
{
    if (disp_fill) {
        for (auto &tri : tris) {
            vvr::Triangle3D(tri, col).draw();
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

/*---[Picking]--------------------------------------------------------------------------*/

struct PaperDragger
{
    vvr_decl_shared_ptr(PaperDragger)
    PaperDragger(vvr::Canvas &sketch) : sketch(sketch) {}
    bool on_pick(vvr::Drawable* drw);
    void on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1);
    void on_drop(vvr::Drawable* drw);
    vvr::Canvas &sketch;
};

bool PaperDragger::on_pick(vvr::Drawable *drw)
{
    return true;
}

void PaperDragger::on_drag(vvr::Drawable *drw, Ray ray0, Ray ray1)
{
    auto paper = static_cast<Paper*>(drw);
    const vec ip = paper->Intersects(std::move(ray1));
    if (ip.IsFinite()) ::append(sketch, ip);
}

void PaperDragger::on_drop(vvr::Drawable *drw)
{
    ::smoothen(sketch);
    for (auto d : sketch.getDrawables()) {
        static_cast<vvr::Shape*>(d)->colour = vvr::LightSeaGreen;
    }
    sketch.newFrame();
}

/*---[Scene]----------------------------------------------------------------------------*/
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
    vvr::KeyMap         m_keymap;
    PickerT::Ptr        m_picker;
    PaperDragger::Ptr   m_dragger;
};

OrigamiScene::OrigamiScene()
{
    m_bg_col = vvr::grey;
    m_perspective_proj = true;

    /* Create drawables and populate canvas. */
    m_paper = Paper::Make(12, 12*math::Sqrt(2));
    m_papers.add(m_paper.get());
    m_papers.setDelOnClear(false);
    m_dragger = PaperDragger::Make(m_sketch);
    m_picker = PickerT::Make(m_papers, m_dragger.get());

    /* Install key bindings */
    m_keymap['a'].add((new vvr::SimpleCmd<vvr::Axes, bool>(&getGlobalAxes(), &vvr::Axes::toggleVisibility)));
    m_keymap['x'].add((new vvr::SimpleCmd<vvr::Canvas>(&m_sketch, &vvr::Canvas::clear)));
    m_keymap['s'].add((new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::toggleFill)));
    m_keymap['w'].add((new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::toggleWire)));
    m_keymap['p'].add((new vvr::SimpleCmd<Paper>(m_paper.get(), &Paper::togglePts)));

    /* Default visibility */
    getGlobalAxes().hide();
}

void OrigamiScene::draw()
{
    getGlobalAxes().drawif();
    m_paper->draw();
    m_sketch.draw();
}

void OrigamiScene::mouseHovered(int x, int y, int modif)
{
    m_picker->pick(unproject(x,y), modif);
}

void OrigamiScene::mousePressed(int x, int y, int modif)
{
    m_picker->pick(unproject(x,y), modif);
    if (!m_picker->picked()) Scene::mousePressed(x, y, modif);
}

void OrigamiScene::mouseMoved(int x, int y, int modif)
{
    m_picker->drag(unproject(x,y), modif);
    if (!m_picker->picked()) Scene::mouseMoved(x, y, modif);
}

void OrigamiScene::mouseReleased(int x, int y, int modif)
{
    m_picker->drop(unproject(x,y), modif);
}

void OrigamiScene::mouseWheel(int dir, int modif)
{
    if (shiftDown(modif)) {
        vvr::Shape::LineWidth += 0.2f*dir;
    } else Scene::mouseWheel(dir, modif);
}

void OrigamiScene::keyEvent(unsigned char key, bool up, int modif)
{
    if(m_keymap.find(key) != std::end(m_keymap)) {
        m_keymap[key]();
    } else Scene::keyEvent(key, up, modif);
}

vvr_invoke_main_with_scene(OrigamiScene)
