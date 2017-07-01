#include "Origami.h"

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
    m_bg_col = vvr::Colour::grey;
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
    const float h = 25;
    const float w = h * math::Sqrt(2); // A4 paper size ratio

    A = math::vec(-w, h, 0);
    B = math::vec(+w, h, 0);
    C = math::vec(+w, -h, 0);
    D = math::vec(-w, -h, 0);
    E = math::vec(-w, h / 2, 0);
    F = math::vec(+0, h, 0);

    Fold f;
    folds.clear();

#ifdef TWOPIECES
    f = Fold();
    f.col = COLMAP[0];
    f.pol.p.push_back(E);
    f.pol.p.push_back(F);
    f.pol.p.push_back(B);
    f.pol.p.push_back(C);
    f.pol.p.push_back(D);
    folds.push_back(f);

    f = Fold();
    f.col = COLMAP[0];
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
            for (auto &t : tri_arr) {
                math2vvr(t, fold.col).draw();
            }
        }

        //! Outline
        if (m_style_flag & SHOW_WIRE) {
            for (int i = 0; i < pol.NumEdges(); ++i) {
                math2vvr(pol.Edge(i), vvr::Colour::black).draw();
            }
        }

        //! Points
        if (m_style_flag & SHOW_POINTS) {
            for (int i = 0; i < pol.NumVertices(); ++i) {
                math2vvr(pol.Vertex(i), vvr::Colour::black).draw();
            }
        }
    }

    m_canvas.draw();

    for (int i = 0; i < m_polygon_set.size(); ++i) {
        vvr::draw(*m_polygon_set.GetAt(i), vvr::Colour::magenta, true);
    }

    vvr::draw(m_polygon, vvr::Colour::orange, true);
}

bool OrigamiScene::idle()
{
    // Check animation alive
    if (!anim.on)
        return false;

    // Find current animation time
    anim.time += (vvr::getSeconds() - anim.last_update);
    anim.last_update = vvr::getSeconds();

    float4x4 T1 = float3x4::Translate(-E);
    float4x4 T2 = float3x4::Translate(E);
    vec dir = vec(F - E).Normalized();
    float rad = math::DegToRad(anim.time * 90);
    float4x4 R = float3x3::RotateAxisAngle(dir, rad);
    float4x4 M = T2*R*T1;
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
        vvr::Shape::DEF_LINE_WIDTH += 0.2f*dir;
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
        float4x4 T1 = float3x4::Translate(-E);
        float4x4 T2 = float3x4::Translate(E);
        vec dir = vec(F - E).Normalized();
        float rad = math::DegToRad(val * 178);
        float4x4 R = float3x3::RotateAxisAngle(dir, rad);
        float4x4 M = T2*R*T1;
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
            const vvr::Colour col = vvr::Colour::red;
            vec p = pol.ClosestPoint(ray.ToLineSegment(10000));
            vector<vvr::Shape*> &shapes = m_canvas.getFrames().back().shapes;
            vvr::Shape *sh;

            // Start new line segment set
            if (shapes.empty() || m_ongoing_slicing_count == 0)
            {
                m_canvas.add(new vvr::LineSeg3D(p.x, p.y, p.z, p.x, p.y, p.z, col));
            }
            else
            {
                auto last_seg = static_cast<vvr::LineSeg3D*>(shapes.back());
                vec last_point(last_seg->x2, last_seg->y2, last_seg->z2);

                if (last_point.Distance(p) < 0.3) break;

                m_canvas.add(new vvr::LineSeg3D(last_seg->x2, last_seg->y2, last_seg->z2, p.x, p.y, p.z, col));

                //! Smoothen slice
                const unsigned N = 4;
                const unsigned shape_num = shapes.size();
                if (shiftDown && shape_num > N && m_ongoing_slicing_count > N + 1)
                {
                    double x = 0; double y = 0; double z = 0;

                    for (int i = 0; i < N; ++i) {
                        vvr::LineSeg3D *lseg = static_cast<vvr::LineSeg3D*>(shapes.at(shape_num - 1 - i));
                        x += lseg->x2;
                        y += lseg->y2;
                        z += lseg->z2;
                    }

                    vvr::LineSeg3D *lseg0 = static_cast<vvr::LineSeg3D*>(shapes.at(shape_num - 2));
                    vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(shapes.at(shape_num - 1));

                    lseg0->x2 = lseg1->x1 = x / N;
                    lseg0->y2 = lseg1->y1 = y / N;
                    lseg0->z2 = lseg1->z1 = z / N;
                }
            }

            ++m_ongoing_slicing_count;
            break;
        }
    }
}

void OrigamiScene::smoothSlices()
{
    unsigned fi = m_canvas.frameIndex();
    m_canvas.newFrame(false);
    vector<vvr::Shape*> &shapes_old = m_canvas.getFrames().at(fi).shapes;
    vector<vvr::Shape*> &shapes = m_canvas.getFrames().at(fi + 1).shapes;

    for (auto sh : shapes_old) {
        m_canvas.add(new vvr::LineSeg3D(*static_cast<vvr::LineSeg3D*>(sh)));
    }

    for (unsigned i = 1; i < shapes.size() - 1; ++i) {
        vvr::LineSeg3D *lseg0 = static_cast<vvr::LineSeg3D*>(shapes.at(i - 1));
        vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(shapes.at(i));
        vvr::LineSeg3D *lseg2 = static_cast<vvr::LineSeg3D*>(shapes.at(i + 1));
        lseg1->x1 = 0.333 * (lseg0->x1 + lseg1->x1 + lseg2->x1);
        lseg1->y1 = 0.333 * (lseg0->y1 + lseg1->y1 + lseg2->y1);
        lseg1->z1 = 0.333 * (lseg0->z1 + lseg1->z1 + lseg2->z1);
    }

    for (unsigned i = 0; i < shapes.size() - 1; ++i) {
        vvr::LineSeg3D *lseg1 = static_cast<vvr::LineSeg3D*>(shapes.at(i));
        vvr::LineSeg3D *lseg2 = static_cast<vvr::LineSeg3D*>(shapes.at(i + 1));
        lseg1->x2 = lseg2->x1;
        lseg1->y2 = lseg2->y1;
        lseg1->z2 = lseg2->z1;
    }
}

void OrigamiScene::undo()
{
    auto &shapes = m_canvas.getFrames().at((m_canvas.frameIndex())).shapes;

    if (shapes.size() > 0) {
        delete shapes.back();
        shapes.resize(shapes.size() - 1);
    }
}

void OrigamiScene::triangulate()
{
    auto &shapes = m_canvas.getFrames().at((m_canvas.frameIndex())).shapes;

    C2DPointSet ptset;

    for (vvr::Shape* sh : shapes) {
        vvr::LineSeg3D &lseg = static_cast<vvr::LineSeg3D&>(*sh);
        ptset.Add(new C2DPoint(lseg.x2, lseg.y2));
    }

    m_polygon.Create(ptset);

    m_polygon.GetConvexSubAreas(m_polygon_set);
}

int main(int argc, char* argv[])
{
    try {
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
