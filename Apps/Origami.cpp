#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/animation.h>
#include <vvr/command.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <unordered_map>

using namespace std;

struct Paper
{
    math::Polygon pol;
    math::float4x4 mat = math::float4x4::identity;
    vvr::Colour col = vvr::Colour("F0E8B6");
    Paper(float w, float h);
};

Paper::Paper(float w, float h)
{
    pol.p.push_back(math::vec(-w, h, 0));
    pol.p.push_back(math::vec(+w, h, 0));
    pol.p.push_back(math::vec(+w, -h, 0));
    pol.p.push_back(math::vec(-w, -h, 0));
}

class OrigamiScene : public vvr::Scene
{
public:
    OrigamiScene();
    const char* getName() const override { return "Origami Scene"; }
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    void draw() override;
    void reset() override;
    void append(int x, int y, bool shiftDown);
    void updateAllCanvas();
    void smooth();
    void triangulate();

private:
    std::vector<Paper> papers;
    C2DPolygon m_polygon;
    C2DPolygonSet m_polygon_set;
    vvr::Animation m_anim;
    unsigned m_num_pts;

private:
    vvr::Axes*  m_axes;
    vvr::Canvas m_sketch;
    vvr::Canvas m_wire;
    vvr::Canvas m_fill;
    vvr::Canvas m_points;

private:
    std::unordered_map<char, vvr::MacroCmd> m_keymap;
};

//! Creation / Callbacks

OrigamiScene::OrigamiScene()
{
    m_bg_col = vvr::grey;
    m_fullscreen = false;
    m_hide_log = false;
    m_hide_sliders = false;
    m_perspective_proj = false;
    m_axes = &getGlobalAxes();
    m_keymap['a'].add((new vvr::SimpleCmd<vvr::Axes, bool>(m_axes, &vvr::Axes::toggleVisibility)));
    m_keymap['s'].add((new vvr::SimpleCmd<vvr::Canvas, bool>(&m_fill, &vvr::Canvas::toggleVisibility)));
    m_keymap['w'].add((new vvr::SimpleCmd<vvr::Canvas, bool>(&m_wire, &vvr::Canvas::toggleVisibility)));
    m_keymap['p'].add((new vvr::SimpleCmd<vvr::Canvas, bool>(&m_points, &vvr::Canvas::toggleVisibility)));
    m_keymap['x'].add((new vvr::SimpleCmd<vvr::Canvas, void>(&m_sketch, &vvr::Canvas::clear)));
    m_keymap['x'].add((new vvr::SimpleCmd<C2DPolygon, void>(&m_polygon, &C2DPolygon::Clear)));
    m_keymap['f'].add((new vvr::SimpleCmd<OrigamiScene, void>(this, &OrigamiScene::smooth)));
    m_keymap['t'].add((new vvr::SimpleCmd<OrigamiScene, void>(this, &OrigamiScene::triangulate)));
    m_keymap[' '].add((new vvr::SimpleCmd<vvr::Animation, bool>(&m_anim, &vvr::Animation::toggle)));
    reset();
}

void OrigamiScene::reset()
{
    Scene::reset();
    m_num_pts = 0;
    papers.clear();
    papers.push_back(Paper(12, 12*math::Sqrt(2)));
    m_anim.reset();
}

void OrigamiScene::draw()
{
    updateAllCanvas();

    m_axes->drawif();
    m_fill.drawif();
    m_wire.drawif();
    m_points.drawif();
    m_sketch.drawif();
}

void OrigamiScene::mousePressed(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down) {
        append(x, y, shift_down);
    }
    else {
        m_num_pts = 0;
        Scene::mousePressed(x, y, modif);
    }
}

void OrigamiScene::mouseReleased(int x, int y, int modif)
{
    Scene::mouseReleased(x, y, modif);
    m_num_pts = 0;
}

void OrigamiScene::mouseMoved(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (m_num_pts) {
        append(x, y, shift_down);
    } else {
        Scene::mouseMoved(x, y, modif);
    }
}

void OrigamiScene::mouseWheel(int dir, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down) {
        Scene::mouseWheel(dir, modif);
    }
    else {
        vvr::Shape::LineWidth += 0.2f*dir;
    }
}

void OrigamiScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    const bool ctrl_down = ctrlDown(modif);
    key = tolower(key);
    if(m_keymap.find(key)!=std::end(m_keymap)) {
        m_keymap[key]();
    } else Scene::keyEvent(key, up, modif);
}

//! Functionality

void OrigamiScene::updateAllCanvas()
{
    m_fill.clear();
    m_wire.clear();
    m_points.clear();

    for (auto &paper : papers)
    {
        auto pol = paper.pol;
        pol.Transform(paper.mat);
        const auto &tri_arr = pol.Triangulate();
        for (auto &tri : tri_arr) {
            m_fill.add(new vvr::Triangle3D(tri, paper.col));
        }
        for (int i = 0; i < pol.NumEdges(); ++i) {
            m_wire.add(new vvr::LineSeg3D(pol.Edge(i), vvr::black));
        }
        for (int i = 0; i < pol.NumVertices(); ++i) {
            m_points.add(new vvr::Point3D(pol.Vertex(i), vvr::black));
        }
    }
}

void OrigamiScene::append(int x, int y, bool shiftDown)
{
    auto ray = unproject(x, y);

    for (auto &paper : papers)
    {
        auto pol = paper.pol;
        pol.Transform(paper.mat);

        if (ray.Intersects(pol))
        {
            const vvr::Colour col = vvr::red;
            math::vec p = pol.ClosestPoint(ray.ToLineSegment(10000));
            vector<vvr::Drawable*> &drawables = m_sketch.getDrawables();

            // Start new line segment set
            if (drawables.empty() || m_num_pts == 0)
            {
                m_sketch.add(new vvr::LineSeg3D(p.x, p.y, p.z, p.x, p.y, p.z, col));
            }
            else
            {
                auto last_seg = static_cast<vvr::LineSeg3D*>(drawables.back());
                math::vec last_point(last_seg->b);

                if (last_point.Distance(p) < 0.3) break;

                m_sketch.add(new vvr::LineSeg3D(math::LineSegment(last_seg->b, p), col));
#if 0
                //! Smoothen slice
                const unsigned n = 4;
                const unsigned sn = drawables.size();
                if (shiftDown && sn > n && m_num_pts > n + 1)
                {
                    math::vec v(0,0,0);
                    for (int i = 0; i < n; ++i) {
                        auto ls = static_cast<vvr::LineSeg3D*>(drawables.at(sn - 1 - i));
                        v += ls->b;
                    }
                    auto ls0 = static_cast<vvr::LineSeg3D*>(drawables.at(sn - 2));
                    auto ls1 = static_cast<vvr::LineSeg3D*>(drawables.at(sn - 1));
                    ls1->a = v / n;
                    ls0->b = ls1->a;
                }
#endif
            }

            ++m_num_pts;
            break;
        }
    }
}

void OrigamiScene::smooth()
{
    if (!m_sketch.size()) return;

    vector<vvr::Drawable*> &drw = m_sketch.getDrawables();

    if (drw.size() < 3) return;

    for (unsigned i = 1; i < drw.size() - 1; ++i) {
        auto ls0 = static_cast<vvr::LineSeg3D*>(drw.at(i - 1));
        auto ls1 = static_cast<vvr::LineSeg3D*>(drw.at(i));
        auto ls2 = static_cast<vvr::LineSeg3D*>(drw.at(i + 1));
        ls1->a = (ls0->a + ls1->a + ls2->a) / 3;
    }

    for (unsigned i = 0; i < drw.size() - 1; ++i) {
        auto ls1 = static_cast<vvr::LineSeg3D*>(drw.at(i));
        auto ls2 = static_cast<vvr::LineSeg3D*>(drw.at(i + 1));
        ls1->b = ls2->a;
    }
}

void OrigamiScene::triangulate()
{
    C2DPointSet ptset;
    for (auto sh : m_sketch.getDrawables()) {
        vvr::LineSeg3D &ls = static_cast<vvr::LineSeg3D&>(*sh);
        ptset.Add(new C2DPoint(ls.b.x, ls.b.y));
    }
    m_polygon.Create(ptset);
    m_polygon.GetConvexSubAreas(m_polygon_set);
}

vvr_invoke_main_with_scene(OrigamiScene)
