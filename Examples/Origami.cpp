#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>

struct Fold
{
    math::Polygon pol;
    math::float4x4 M = math::float4x4::identity;
    vvr::Colour col;
};

struct Animation
{
    bool on;
    float time;
    float last_update;
    float speed = 1;
};

class OrigamiScene : public vvr::Scene
{
public:
    OrigamiScene();
    const char* getName() const { return "Origami Scene"; }
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void sliderChanged(int slider_id, float val) override;

protected:
    void draw() override;
    void reset() override;
    bool idle() override;

private:
    void makePaper();
    void newLineSegment(int x, int y, bool shiftDown);
    void smoothSlices();
    void triangulate();
    void undo();

private:
    vvr::Canvas m_canvas;
    std::vector<Fold> folds;
    math::vec A, B, C, D, E, F;
    C2DPolygon m_polygon;
    C2DPolygonSet m_polygon_set;
    Animation anim;
    unsigned m_ongoing_slicing_count;
    int m_style_flag;
};

using namespace std;

#define SHOW_AXES 1
#define SHOW_AABB 2
#define SHOW_WIRE 4
#define SHOW_SOLID 8
#define SHOW_POINTS 16
#define SHOW_NORMALS 32

#define TWOPIECES 1

const vvr::Colour COLMAP[] = {
    vvr::Colour("F0E8B6"),
    vvr::Colour("F2E383"),
    vvr::Colour("F2DD55"),
};

OrigamiScene::OrigamiScene()
{
    m_bg_col = vvr::grey;
    m_fullscreen = false;
    m_hide_log = false;
    m_perspective_proj = true;
    m_style_flag = 0;
    m_style_flag |= SHOW_SOLID;
    m_style_flag |= SHOW_POINTS;
    reset();
}

void OrigamiScene::reset()
{
    Scene::reset();
    m_ongoing_slicing_count = 0;
    makePaper();
    anim.on = false;
    anim.speed = 1;
    anim.time = 0;
    anim.last_update = vvr::getSeconds();
}

void OrigamiScene::makePaper()
{
    const float h = 15;
    const float w = h * math::Sqrt(2); // A4 paper size ratio

    A = math::vec(-w, h, 0);
    B = math::vec(+w, h, 0);
    C = math::vec(+w, -h, 0);
    D = math::vec(-w, -h, 0);
    E = math::vec(-w, h / 2, 0);
    F = math::vec(+0, h, 0);

    Fold f;
    folds.clear();
    size_t col_index = 0;

#if(TWOPIECES)
    f = Fold();
    f.col = COLMAP[col_index];
    f.pol.p.push_back(E);
    f.pol.p.push_back(F);
    f.pol.p.push_back(B);
    f.pol.p.push_back(C);
    f.pol.p.push_back(D);
    folds.push_back(f);

    f = Fold();
    f.col = COLMAP[col_index];
    f.pol.p.push_back(A);
    f.pol.p.push_back(F);
    f.pol.p.push_back(E);
    folds.push_back(f);
#else
    f = Fold();
    f.col = COLMAP[0];
    f.pol.p.push_back(A);
    f.pol.p.push_back(B);
    f.pol.p.push_back(C);
    f.pol.p.push_back(D);
    folds.push_back(f);
#endif
}

void OrigamiScene::draw()
{
    if (m_style_flag & SHOW_AXES) {
        drawAxes();
    }

    for (auto &fold : folds)
    {
        auto pol = fold.pol;
        pol.Transform(fold.M);

        //! Fill
        if (m_style_flag & SHOW_SOLID) {
            const auto &tri_arr = pol.Triangulate();
            for (auto &tri : tri_arr) {
                vvr::Triangle3D(tri, fold.col).draw();
            }
        }

        //! Outline
        if (m_style_flag & SHOW_WIRE) {
            for (int i = 0; i < pol.NumEdges(); ++i) {
                math2vvr(pol.Edge(i), vvr::black).draw();
            }
        }

        //! Points
        if (m_style_flag & SHOW_POINTS) {
            for (int i = 0; i < pol.NumVertices(); ++i) {
                math2vvr(pol.Vertex(i), vvr::black).draw();
            }
        }
    }

    m_canvas.draw();

    for (int i = 0; i < m_polygon_set.size(); ++i) {
        vvr::draw(*m_polygon_set.GetAt(i), vvr::magenta, true);
    }

    vvr::draw(m_polygon, vvr::orange, true);
}

bool OrigamiScene::idle()
{
    // Check animation alive
    if (!anim.on)
        return false;

    // Find current animation time
    anim.time += (vvr::getSeconds() - anim.last_update);
    anim.last_update = vvr::getSeconds();

    math::float4x4 T1 = math::float3x4::Translate(-E);
    math::float4x4 T2 = math::float3x4::Translate(E);
    math::vec dir = math::vec(F - E).Normalized();
    float rad = math::DegToRad(anim.time * 90);
    math::float4x4 R = math::float3x3::RotateAxisAngle(dir, rad);
    math::float4x4 M = T2*R*T1;
    folds.at(1).M = M;
}

void OrigamiScene::mousePressed(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down && !m_ongoing_slicing_count)
    {
        return Scene::mousePressed(x, y, modif);
    }
    else
    {
        newLineSegment(x, y, shift_down);
    }
}

void OrigamiScene::mouseReleased(int x, int y, int modif)
{
    Scene::mouseReleased(x, y, modif);
    m_ongoing_slicing_count = 0;
}

void OrigamiScene::mouseMoved(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down && !m_ongoing_slicing_count)
    {
        return Scene::mouseMoved(x, y, modif);
    }
    else
    {
        newLineSegment(x, y, shift_down);
    }
}

void OrigamiScene::mouseWheel(int dir, int modif)
{
    if (!shiftDown(modif)) {
        Scene::mouseWheel(dir, modif);
    }
    else {
        vvr::Shape::LineWidth += 0.2f*dir;
    }
}

void OrigamiScene::sliderChanged(int slider_id, float val)
{
    switch (slider_id)
    {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
    {
        math::float4x4 T1 = math::float3x4::Translate(-E);
        math::float4x4 T2 = math::float3x4::Translate(E);
        math::vec dir = math::vec(F - E).Normalized();
        float rad = math::DegToRad(val * 178);
        math::float4x4 R = math::float3x3::RotateAxisAngle(dir, rad);
        math::float4x4 M = T2*R*T1;
        folds.at(1).M = M;
        break;
    }
    }
}

void OrigamiScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    const bool ctrl_down = ctrlDown(modif);
    const bool alt_down = altDown(modif);
    const bool shift_down = shiftDown(modif);
    key = tolower(key);

    switch (key)
    {
    case 'a': m_style_flag ^= SHOW_AXES; break;
    case 'w': m_style_flag ^= SHOW_WIRE; break;
    case 's': m_style_flag ^= SHOW_SOLID; break;
    case 'n': m_style_flag ^= SHOW_NORMALS; break;
    case 'p': m_style_flag ^= SHOW_POINTS; break;
    case 'b': m_style_flag ^= SHOW_AABB; break;
    case ' ': anim.on = !anim.on; if (anim.on) { anim.last_update = vvr::getSeconds(); }break;
    case 'x': m_canvas.clear(); m_polygon.Clear(); m_polygon_set.DeleteAll(); break;
    case 'f': smoothSlices(); break;
    case 'z': if (ctrl_down) undo(); break;
    case 't': triangulate(); break;
    }
}

void OrigamiScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT)
    {
        m_canvas.prev();
    }
    else if (dir == vvr::RIGHT)
    {
        m_canvas.next();
    }
}

void OrigamiScene::newLineSegment(int x, int y, bool shiftDown)
{
    auto ray = unproject(x, y);

    for (auto &fold : folds)
    {
        auto pol = fold.pol;
        pol.Transform(fold.M);

        if (ray.Intersects(pol))
        {
            const vvr::Colour col = vvr::red;
            math::vec p = pol.ClosestPoint(ray.ToLineSegment(10000));
            vector<vvr::Drawable*> &drawables = m_canvas.getDrawables();
            vvr::Drawable *sh;

            // Start new line segment set
            if (drawables.empty() || m_ongoing_slicing_count == 0)
            {
                m_canvas.add(new vvr::LineSeg3D(p.x, p.y, p.z, p.x, p.y, p.z, col));
            }
            else
            {
                auto last_seg = static_cast<vvr::LineSeg3D*>(drawables.back());
                math::vec last_point(last_seg->b);

                if (last_point.Distance(p) < 0.3) break;

                m_canvas.add(new vvr::LineSeg3D(math::LineSegment(last_seg->b, p), col));

                //! Smoothen slice
                const unsigned N = 4;
                const unsigned shape_num = drawables.size();
                if (shiftDown && shape_num > N && m_ongoing_slicing_count > N + 1)
                {
                    math::vec v(0,0,0);

                    for (int i = 0; i < N; ++i) {
                        vvr::LineSeg3D *lseg = static_cast<vvr::LineSeg3D*>(drawables.at(shape_num - 1 - i));
                        v += lseg->b;
                    }

                    vvr::LineSeg3D *lseg0 = static_cast<vvr::LineSeg3D*>(drawables.at(shape_num - 2));
                    vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(drawables.at(shape_num - 1));
                    lseg1->a = v / N;
                    lseg0->b = lseg1->a;
                }
            }

            ++m_ongoing_slicing_count;
            break;
        }
    }
}

void OrigamiScene::smoothSlices()
{
    unsigned fid = m_canvas.frameIndex();
    m_canvas.newFrame(false);
    vector<vvr::Drawable*> &drawables = m_canvas.getDrawables(0);
    vector<vvr::Drawable*> &drawables_prev = m_canvas.getDrawables(-1);

    for (auto sh : drawables_prev) {
        m_canvas.add(new vvr::LineSeg3D(*static_cast<vvr::LineSeg3D*>(sh)));
    }

    for (unsigned i = 1; i < drawables.size() - 1; ++i) {
        vvr::LineSeg3D *lseg0 = static_cast<vvr::LineSeg3D*>(drawables.at(i - 1));
        vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(drawables.at(i));
        vvr::LineSeg3D *lseg2 = static_cast<vvr::LineSeg3D*>(drawables.at(i + 1));
        lseg1->a = 0.333 * (lseg0->a + lseg1->a + lseg2->a);
    }

    for (unsigned i = 0; i < drawables.size() - 1; ++i) {
        vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(drawables.at(i));
        vvr::LineSeg3D *lseg2 = static_cast<vvr::LineSeg3D*>(drawables.at(i + 1));
        lseg1->b = lseg2->a;
    }
}

void OrigamiScene::undo()
{
    auto &drawables = m_canvas.getDrawables();

    if (drawables.size() > 0) {
        delete drawables.back();
        drawables.resize(drawables.size() - 1);
    }
}

void OrigamiScene::triangulate()
{
    auto &drawables = m_canvas.getDrawables();

    C2DPointSet ptset;

    for (vvr::Drawable* sh : drawables) {
        vvr::LineSeg3D &lseg = static_cast<vvr::LineSeg3D&>(*sh);
        ptset.Add(new C2DPoint(lseg.b.x, lseg.b.y));
    }

    m_polygon.Create(ptset);

    m_polygon.GetConvexSubAreas(m_polygon_set);
}

int main(int argc, char* argv[])
{
    try 
    {
        return vvr::mainLoop(argc, argv, new OrigamiScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception" << endl;
    }
    return 1;
}
